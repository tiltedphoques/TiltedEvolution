#include "AlchemyItem.h"
#include <Games/BGSCreatedObjectManager.h>
#include <Systems/ModSystem.h>
#include <TESObjectREFR.h>
#include <World.h>

bool AlchemyItem::CaptureRecipe(TESForm* apForm, Inventory::PotionData& aData) noexcept
{
    aData = {};
    auto* potion = apForm && apForm->IsTemporary() ? Cast<AlchemyItem>(apForm) : nullptr;
    if (!potion || potion->IsFood())
        return false;
    aData.IsPoison = potion->IsPoison();
    auto& mods = World::Get().GetModSystem();
    for (auto* effect : potion->listOfEffects)
    {
        Inventory::EffectItem result;
        // Conditions and temporary magic effects cannot be reproduced faithfully.
        if (!effect || !effect->pEffectSetting || effect->Condition.pHead || effect->pEffectSetting->IsTemporary() ||
            !mods.GetServerModId(effect->pEffectSetting->formID, result.EffectId))
        {
            aData.Effects.clear();
            return false;
        }
        result.Magnitude = effect->data.fMagnitude;
        result.Area = effect->data.iArea;
        result.Duration = effect->data.iDuration;
        result.RawCost = effect->fRawCost;
        aData.Effects.push_back(result);
    }
    if (!aData.IsValid())
        aData.Effects.clear();
    return !aData.Effects.empty();
}

void AlchemyItem::Capture(TESForm* apForm, Inventory::Entry& aEntry) noexcept
{
    auto* potion = apForm && apForm->IsTemporary() ? Cast<AlchemyItem>(apForm) : nullptr;
    if (!potion || potion->IsFood())
        return;
    CaptureRecipe(apForm, aEntry.Potion);
}

AlchemyItem* AlchemyItem::Find(TESObjectREFR* apOwner, const Inventory::PotionData& aData) noexcept
{
    auto* changes = apOwner ? apOwner->GetContainerChanges() : nullptr;
    if (!changes || !changes->entries || !aData.IsValid())
        return nullptr;
    for (auto* entry : *changes->entries)
    {
        if (!entry || !entry->form || entry->count <= 0)
            continue;
        Inventory::Entry local;
        Capture(entry->form, local);
        if (local.Potion == aData)
            return Cast<AlchemyItem>(entry->form);
    }
    return nullptr;
}

namespace
{
// Effect cost only feeds the item's gold value; the engine may keep its own when it reuses a created potion.
bool SameEffects(const Inventory::PotionData& acLhs, const Inventory::PotionData& acRhs) noexcept
{
    return acLhs.IsPoison == acRhs.IsPoison && acLhs.Effects.size() == acRhs.Effects.size() &&
        std::is_permutation(acLhs.Effects.begin(), acLhs.Effects.end(), acRhs.Effects.begin(), [](const auto& acA, const auto& acB) {
            return acA.EffectId == acB.EffectId && acA.Magnitude == acB.Magnitude && acA.Area == acB.Area && acA.Duration == acB.Duration;
        });
}
} // namespace

AlchemyItem* AlchemyItem::Create(const Inventory::PotionData& aData) noexcept
{
    if (!aData.IsValid())
        return nullptr;
    auto* manager = BGSCreatedObjectManager::Get();
    if (!manager)
        return nullptr;
    auto& mods = World::Get().GetModSystem();
    GameArray<EffectItem> effects;
    effects.Resize(aData.Effects.size());
    for (uint32_t i = 0; i < aData.Effects.size(); ++i)
    {
        const auto& source = aData.Effects[i];
        EffectItem effect{};
        effect.data = {source.Magnitude, source.Area, source.Duration};
        effect.fRawCost = source.RawCost;
        effect.pEffectSetting = Cast<EffectSetting>(TESForm::GetById(mods.GetGameId(source.EffectId)));
        if (!effect.pEffectSetting)
        {
            Memory::Free(effects.data);
            return nullptr;
        }
        effects[i] = effect;
    }
    // CommonLibSSE AddPotion/AddPoison: output is a one-pointer created-object smart pointer.
    // AE address-library IDs 36167 (potion) and 36168 (poison), the same calls the alchemy menu ends with.
    // No AlchemyMenu, recipe, ingredient or skill calculation is involved.
    TP_THIS_FUNCTION(TAddPotion, void, BGSCreatedObjectManager, AlchemyItem**, GameArray<EffectItem>*);
    POINTER_SKYRIMSE(TAddPotion, addPotion, 36167);
    POINTER_SKYRIMSE(TAddPotion, addPoison, 36168);
    AlchemyItem* created = nullptr;
    TiltedPhoques::ThisCall(aData.IsPoison ? addPoison : addPotion, manager, &created, &effects);
    Memory::Free(effects.data);
    if (!created)
    {
        spdlog::error("[TradeService]: {} returned no form for a {}-effect recipe", aData.IsPoison ? "AddPoison" : "AddPotion", aData.Effects.size());
        return nullptr;
    }
    Inventory::PotionData check;
    CaptureRecipe(created, check);
    if (SameEffects(check, aData))
        return created;
    // The engine may hand back an existing created potion, including one in this player's own inventory.
    // Dropping its reference here could free a form an inventory still holds, so it is only refused.
    spdlog::error("[TradeService]: {} returned {:X} whose effects differ from the offered recipe", aData.IsPoison ? "AddPoison" : "AddPotion", created->formID);
    for (const auto& effect : aData.Effects)
        spdlog::error("[TradeService]:   wanted {:X}:{:X} magnitude {} area {} duration {} cost {}", effect.EffectId.ModId, effect.EffectId.BaseId, effect.Magnitude, effect.Area, effect.Duration, effect.RawCost);
    for (const auto& effect : check.Effects)
        spdlog::error("[TradeService]:   got {:X}:{:X} magnitude {} area {} duration {} cost {}", effect.EffectId.ModId, effect.EffectId.BaseId, effect.Magnitude, effect.Area, effect.Duration, effect.RawCost);
    return nullptr;
}
