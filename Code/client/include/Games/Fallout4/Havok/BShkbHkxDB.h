#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Havok/hkHashTable.h>

struct BShkbHkxDB
{
    virtual ~BShkbHkxDB();

    uint8_t pad8[0x84 - 0x8];
    hkHashTable animationVariables; // 84
};

static_assert(offsetof(BShkbHkxDB, animationVariables) == 0x84);

#endif
