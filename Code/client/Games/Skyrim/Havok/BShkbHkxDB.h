#pragma once

#include <Havok/hkHashTable.h>

struct BShkbHkxDB
{
    virtual ~BShkbHkxDB();

    uint8_t pad8[0x7C - 0x8];
    hkHashTable animationVariables; // 7C
    uint8_t padA0[0xC];
    hkHashTable hashTable; // AC
    uint8_t padD0[0x160 - 0xD0];
    void* ptr160;
};

static_assert(offsetof(BShkbHkxDB, animationVariables) == 0x7C);
static_assert(offsetof(BShkbHkxDB, hashTable) == 0xAC);
