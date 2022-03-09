#pragma once

#include "MagicItem.h"

#include <Structs/Inventory.h>
#include <Games/Magic/MagicSystem.h>
#include "BGSListForm.h"

struct EnchantmentItem : MagicItem
{
    static EnchantmentItem* Create(const Inventory::EnchantmentData& aData) noexcept;

    int32_t iCostOverride;
    int32_t iFlags;
    MagicSystem::CastingType eCastingType;
    int32_t iChargeOverride;
    MagicSystem::Delivery eDelivery;
    MagicSystem::SpellType eSpellType;
    float fChargeTime;
    EnchantmentItem* pBaseEnchantment;
    BGSListForm* pWornRestrictions;
};

static_assert(sizeof(EnchantmentItem) == 0xC0);
