#include "EnchantmentItem.h"

#include <Systems/ModSystem.h>
#include <Magic/EffectItem.h>
#include <World.h>

EnchantmentItem* EnchantmentItem::Create(const Container::EnchantmentData& aData) noexcept
{
    TP_THIS_FUNCTION(TCreateNewEnchantment, EnchantmentItem*, GameArray<EffectItem>, bool abIsWeapon);
    POINTER_SKYRIMSE(TCreateNewEnchantment, createNewEnchantment, 0x1405C1290 - 0x140000000);

    ModSystem& modSystem = World::Get().GetModSystem();

    GameArray<EffectItem> effects{};
    effects.Resize(aData.Effects.size());
    for (int i = 0; i < aData.Effects.size(); i++)
    {
        Container::EffectItem effect = aData.Effects[i];

        EffectItem effectItem{};
        effectItem.data.fMagnitude = effect.Magnitude;
        effectItem.data.iArea = effect.Area;
        effectItem.data.iDuration = effect.Duration;
        effectItem.fRawCost = effect.RawCost;
        effectItem.pEffectSetting =
            RTTI_CAST(TESForm::GetById(modSystem.GetGameId(effect.EffectId)), TESForm, EffectSetting);
        if (!effectItem.pEffectSetting)
            spdlog::error("Effect setting not found: {:X}:{:X}", effect.EffectId.ModId, effect.EffectId.BaseId);

        // TODO: TESCondition

        effects[i] = effectItem;
    }

    EnchantmentItem* pItem = ThisCall(createNewEnchantment, &effects, aData.IsWeapon);

    return pItem;
}

void EnchantmentItem::Init(const Container::EnchantmentData& aData)
{
    /*
    iCostOverride = aData.CostOverride;
    iFlags = aData.Flags;
    eCastingType = static_cast<MagicSystem::CastingType>(aData.CastingType);
    iChargeOverride = aData.ChargeOverride;
    eDelivery = static_cast<MagicSystem::Delivery>(aData.Delivery);
    eSpellType = static_cast<MagicSystem::SpellType>(aData.SpellType);
    fChargeTime = aData.ChargeTime;

    ModSystem& modSystem = World::Get().GetModSystem();

    TP_ASSERT(aData.BaseEnchantmentId.ModId != 0xFFFFFFFF, "Base enchantment is a temporary!");
    uint32_t baseEnchantId = modSystem.GetGameId(aData.BaseEnchantmentId);
    pBaseEnchantment = RTTI_CAST(TESForm::GetById(baseEnchantId), TESForm, EnchantmentItem);
    if (!pBaseEnchantment)
        spdlog::error("{}: base enchantment not found.", __FUNCTION__);

    TP_ASSERT(aData.WornRestrictionsId.ModId != 0xFFFFFFFF, "Worn restrictions is a temporary!");
    uint32_t restrictionsId = modSystem.GetGameId(aData.WornRestrictionsId);
    pWornRestrictions = RTTI_CAST(TESForm::GetById(restrictionsId), TESForm, BGSListForm);
    if (!pWornRestrictions)
        spdlog::error("{}: worn restrictions not found.", __FUNCTION__);
    */
}
