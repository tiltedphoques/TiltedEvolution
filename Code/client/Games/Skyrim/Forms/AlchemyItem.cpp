#include "AlchemyItem.h"
#include <Games/BGSCreatedObjectManager.h>
#include <Systems/ModSystem.h>
#include <TESObjectREFR.h>
#include <World.h>

void AlchemyItem::Capture(TESForm* apForm, Inventory::Entry& aEntry) noexcept
{
    auto* potion = apForm && apForm->IsTemporary() ? Cast<AlchemyItem>(apForm) : nullptr;
    if (!potion || potion->IsFood())
        return;
    aEntry.Potion = {};
    aEntry.Potion.IsPoison = potion->IsPoison();
    auto& mods = World::Get().GetModSystem();
    for (auto* effect : potion->listOfEffects)
    {
        Inventory::EffectItem result;
        // Conditions and temporary magic effects cannot be reproduced faithfully.
        if (!effect || !effect->pEffectSetting || effect->Condition.pHead || effect->pEffectSetting->IsTemporary() ||
            !mods.GetServerModId(effect->pEffectSetting->formID, result.EffectId))
        {
            aEntry.Potion.Effects.clear();
            return;
        }
        result.Magnitude = effect->data.fMagnitude;
        result.Area = effect->data.iArea;
        result.Duration = effect->data.iDuration;
        result.RawCost = effect->fRawCost;
        aEntry.Potion.Effects.push_back(result);
    }
    if (!aEntry.Potion.IsValid())
        aEntry.Potion.Effects.clear();
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
    if (created)
    {
        Inventory::Entry check;
        Capture(created, check);
        if (check.Potion == aData)
            return created;
        spdlog::error("[TradeService]: AddPotion returned effects/type differing from the offered potion");
        Release(created);
    }
    return nullptr;
}

void AlchemyItem::Release(AlchemyItem* apItem) noexcept
{
    auto* manager = BGSCreatedObjectManager::Get();
    if (!apItem || !manager)
        return;
    TP_THIS_FUNCTION(TDecrementRef, void, BGSCreatedObjectManager, AlchemyItem*);
    POINTER_SKYRIMSE(TDecrementRef, decrementRef, 36171);
    TiltedPhoques::ThisCall(decrementRef, manager, apItem);
}
