#pragma once

struct BGSAction;

struct DefaultObjectManager
{
    static DefaultObjectManager& Get();

    uint8_t padB8[0xB8];
    void* leftEquipSlot;
    void* rightEquipSlot;
    uint8_t pad0[0x220 - 0xC8];
    BGSAction* someAction;  // 220
    uint8_t pad228[0xBC0 - 0x228];
    bool isSomeActionReady; // BC0
};

static_assert(offsetof(DefaultObjectManager, leftEquipSlot) == 0xB8);
static_assert(offsetof(DefaultObjectManager, rightEquipSlot) == 0xC0);
static_assert(offsetof(DefaultObjectManager, someAction) == 0x220);
static_assert(offsetof(DefaultObjectManager, isSomeActionReady) == 0xBC0);
