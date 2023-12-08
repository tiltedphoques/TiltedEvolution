#pragma once

#include <BSCore/BSTArray.h>

struct TESBoundObject;
struct TBO_InstanceData;

struct BGSInventoryList
{
    struct BGSInventoryItem
    {
        TESBoundObject* pObject;
        TBO_InstanceData* pInstances;
        uint32_t count;
        uint32_t capacity;
        void* unk18;
        uint32_t unk20;
        uint32_t unk24;
    };

    virtual ~BGSInventoryList();

    uint8_t unk8[0x28 - 0x8];
    uint32_t uniqueItemCount; // 0x28
    uint32_t relatedToUniqueItemCount; // 0x2C
    BGSInventoryItem* inventoryItems; // 0x30
    uint32_t unk38; // 0x38
    float fWeight; // 0x3C
};

static_assert(sizeof(BGSInventoryList) == 0x40);
static_assert(sizeof(BGSInventoryList::BGSInventoryItem) == 0x28);
