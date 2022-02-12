#include "EnchantmentItem.h"

#include <Systems/ModSystem.h>
#include <Magic/EffectItem.h>
#include <World.h>

EnchantmentItem* EnchantmentItem::Create(const Container::EnchantmentData& aData) noexcept
{
    TP_THIS_FUNCTION(TCreateNewEnchantment, EnchantmentItem*, GameArray<EffectItem*>, bool abIsWeapon);
    POINTER_SKYRIMSE(TCreateNewEnchantment, createNewEnchantment, 0x1405C1290 - 0x140000000);

    ModSystem& modSystem = World::Get().GetModSystem();

    GameArray<EffectItem*> effects{};
    for (Container::EffectItem effect : aData.Effects)
    {
        EffectItem* pEffectItem = new EffectItem;
        pEffectItem->data.fMagnitude = effect.Magnitude;
        pEffectItem->data.iArea = effect.Area;
        pEffectItem->data.iDuration = effect.Duration;
        pEffectItem->fRawCost = effect.RawCost;
        pEffectItem->pEffectSetting =
            RTTI_CAST(TESForm::GetById(modSystem.GetGameId(effect.EffectId)), TESForm, EffectSetting);
        if (!pEffectItem->pEffectSetting)
            spdlog::error("Effect setting not found: {:X}:{:X}", effect.EffectId.ModId, effect.EffectId.BaseId);
    }

    EnchantmentItem* pItem = ThisCall(createNewEnchantment, &effects, aData.IsWeapon);

    for (EffectItem* pEffectItem : effects)
        delete pEffectItem;

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
