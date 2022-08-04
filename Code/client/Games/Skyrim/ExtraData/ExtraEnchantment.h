#pragma once

#include "ExtraData.h"

struct EnchantmentItem;

struct ExtraEnchantment : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Enchantment;

    EnchantmentItem* pEnchantment;
    uint16_t usCharge;
    bool bRemoveOnUnequip;
};

static_assert(sizeof(ExtraEnchantment) == 0x20);

