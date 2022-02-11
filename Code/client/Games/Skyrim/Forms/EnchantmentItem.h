#pragma once

#include "TESForm.h"

#include <Structs/Container.h>
#include <Games/Magic/MagicSystem.h>

struct EnchantmentItem : TESForm
{
    void Init(const Container::EnchantmentData& aData);

    MagicSystem::CastingType eCastingType;
    int32_t iChargeOverride;
    MagicSystem::Delivery eDelivery;
    MagicSystem::SpellType eSpellType;
    float fChargeTime;
    EnchantmentItem* pBaseEnchantment;
    void* pWornRestrictions;
};
