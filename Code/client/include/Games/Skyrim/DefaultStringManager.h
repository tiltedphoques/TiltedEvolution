#pragma once

#if TP_SKYRIM64

#include <Games/Skyrim/Misc/BSFixedString.h>

struct DefaultStringManager
{
    static DefaultStringManager& Get();

    uint8_t pad0[0x3B8];
    BSFixedString fIdleTimer;  // 3B8
};

static_assert(offsetof(DefaultStringManager, fIdleTimer) == 0x3B8);

#endif
