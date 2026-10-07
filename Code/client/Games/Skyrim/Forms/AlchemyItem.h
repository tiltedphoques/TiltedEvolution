#pragma once

#include "MagicItem.h"
#include <Structs/Inventory.h>

struct TESObjectREFR;

struct AlchemyItem : MagicItem
{
    enum AlchemyFlags : uint32_t
    {
        Food = 1 << 1,
        Poison = 1 << 17,
    };

    struct Data
    {
        int32_t costOverride;
        uint32_t flags;
        void* pAddiction;
        float addictionChance;
        uint32_t pad14;
        void* pConsumptionSound;
    };

    // ENIT layout: https://github.com/CharmedBaryon/CommonLibSSE-NG/blob/main/include/RE/A/AlchemyItem.h
    uint8_t pad90[0x138 - sizeof(MagicItem)];
    Data data;
    uint8_t pad158[0x10];

    bool IsPoison() const noexcept { return (data.flags & Poison) != 0; }
    bool IsFood() const noexcept { return (data.flags & Food) != 0 && !IsPoison(); }
    // Records a player-crafted potion or poison's effects; false for other forms or unsupported effects.
    static bool CaptureRecipe(TESForm* apForm, Inventory::PotionData& aData) noexcept;
    static void Capture(TESForm* apForm, Inventory::Entry& aEntry) noexcept;
    static AlchemyItem* Find(TESObjectREFR* apOwner, const Inventory::PotionData& aData) noexcept;
    // The returned form may be an existing created potion with the same effects. Its created-object
    // reference is never dropped: other inventories can point at the same form.
    static AlchemyItem* Create(const Inventory::PotionData& aData) noexcept;
};

static_assert(sizeof(AlchemyItem::Data) == 0x20);
static_assert(offsetof(AlchemyItem, data) == 0x138);
static_assert(sizeof(AlchemyItem) == 0x168);
