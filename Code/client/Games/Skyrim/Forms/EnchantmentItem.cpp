#include "EnchantmentItem.h"

#include <Systems/ModSystem.h>
#include <Magic/EffectItem.h>
#include <World.h>
#include <Games/BGSCreatedObjectManager.h>

EnchantmentItem* EnchantmentItem::Create(const Inventory::EnchantmentData& aData) noexcept
{
    TP_THIS_FUNCTION(TAddWeaponEnchantment, EnchantmentItem*, void, GameArray<EffectItem>*);
    POINTER_SKYRIMSE(TAddWeaponEnchantment, addWeaponEnchantment, 36165);
    TP_THIS_FUNCTION(TAddArmorEnchantment, EnchantmentItem*, void, GameArray<EffectItem>*);
    POINTER_SKYRIMSE(TAddArmorEnchantment, addArmorEnchantment, 36166);

    ModSystem& modSystem = World::Get().GetModSystem();

    // Effects from a mod this client doesn't have resolve to null. Keep only the ones that exist here; a created
    // enchantment with a null effect crashes the game as soon as it is displayed, equipped or applied.
    Vector<EffectItem> resolvedEffects;
    resolvedEffects.reserve(aData.Effects.size());
    for (const Inventory::EffectItem& effect : aData.Effects)
    {
        auto* pEffectSetting = Cast<EffectSetting>(TESForm::GetById(modSystem.GetGameId(effect.EffectId)));
        if (!pEffectSetting)
        {
            spdlog::warn("Dropping enchantment effect {:X}:{:X}, not found on this client (missing mod?)", effect.EffectId.ModId, effect.EffectId.BaseId);
            continue;
        }

        EffectItem effectItem{};
        effectItem.data.fMagnitude = effect.Magnitude;
        effectItem.data.iArea = effect.Area;
        effectItem.data.iDuration = effect.Duration;
        effectItem.fRawCost = effect.RawCost;
        effectItem.pEffectSetting = pEffectSetting;

        // TODO: TESCondition?

        resolvedEffects.push_back(effectItem);
    }

    if (resolvedEffects.empty())
    {
        spdlog::warn("Not creating enchantment, none of its {} effects exist on this client", aData.Effects.size());
        return nullptr;
    }

    GameArray<EffectItem> effects{};
    effects.Resize(resolvedEffects.size());
    for (size_t i = 0; i < resolvedEffects.size(); i++)
        effects[i] = resolvedEffects[i];

    BGSCreatedObjectManager* objManager = BGSCreatedObjectManager::Get();

    EnchantmentItem* pItem = nullptr;
    if (aData.IsWeapon)
    {
        pItem = TiltedPhoques::ThisCall(addWeaponEnchantment, objManager, &effects);
    }
    else
    {
        pItem = TiltedPhoques::ThisCall(addArmorEnchantment, objManager, &effects);
    }

    return pItem;
}
