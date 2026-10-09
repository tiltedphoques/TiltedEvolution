#pragma once

#include <NetImmerse/NiPoint3.h>

struct TESForm;

// Fallout 4 EquippedItem: 0x28.
struct EquippedItem
{
    TESForm* object;           // 0x00 BGSObjectInstance
    void* instanceData;        // 0x08
    TESForm* equipSlot;        // 0x10 BGSEquipSlot
    uint32_t equipIndex;       // 0x18
    void* data;                // 0x20 NiPointer<EquippedItemData>
};
static_assert(sizeof(EquippedItem) == 0x28);

// Fallout 4 MiddleHighProcessData: 0x4C0. Only the fields the client uses.
struct MiddleProcess
{
    uint8_t pad0[0xB8];
    float pitch;     // 0xB8 rotation.x
    float roll;      // 0xBC rotation.y
    float direction; // 0xC0 rotation.z, AIProcess::GetDirection
    uint8_t padC4[0x288 - 0xC4];
    GameArray<EquippedItem> equippedItems; // 0x288
    uint8_t pad2A0[0x3B8 - 0x2A0];
    BSPointerHandle<TESObjectREFR> commandingActor; // 0x3B8
    uint8_t pad3BC[0x4C0 - 0x3BC];
};

static_assert(offsetof(MiddleProcess, direction) == 0xC0);
static_assert(offsetof(MiddleProcess, equippedItems) == 0x288);
static_assert(offsetof(MiddleProcess, commandingActor) == 0x3B8);
static_assert(sizeof(MiddleProcess) == 0x4C0);
