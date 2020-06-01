#pragma once

#if TP_SKYRIM64

struct BGSAction;

struct DefaultObjectManager
{
    static DefaultObjectManager& Get();

    uint8_t pad0[0x220];
    BGSAction* someAction;  // 220
    uint8_t pad228[0xBC0 - 0x228];
    bool isSomeActionReady; // BC0
};

static_assert(offsetof(DefaultObjectManager, someAction) == 0x220);
static_assert(offsetof(DefaultObjectManager, isSomeActionReady) == 0xBC0);

#endif
