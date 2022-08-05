#pragma once

#include "MagicItem.h"

#include <Structs/Inventory.h>

struct AlchemyItem : MagicItem
{
    static AlchemyItem* Create(const Inventory::EnchantmentData& aData) noexcept;

    uint8_t unk90[0x138 - 0x90];
    int32_t costOverride;
    uint32_t flags;
    void* pUnk1;
    float fUnk2;
    void* pUnk3;
    uint8_t unk158[0x10];
};

static_assert(sizeof(AlchemyItem) == 0x168);
