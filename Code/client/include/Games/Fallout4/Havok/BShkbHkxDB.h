#pragma once

#include <Havok/hkHashTable.h>

struct BShkbHkxDB
{
    virtual ~BShkbHkxDB();

    uint8_t pad8[0x84 - 0x8];
    hkHashTable animationVariables; // 84
};

static_assert(offsetof(BShkbHkxDB, animationVariables) == 0x84);
