#pragma once

#include <Games/ExtraData.h>

struct EnchantmentItem;

struct ExtraEnchantment : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Enchantment;

    EnchantmentItem* pEnchantment;
    uint16_t usCharge;
    bool bRemoveOnUnequip;
};

static_assert(sizeof(ExtraEnchantment) == 0x20);

