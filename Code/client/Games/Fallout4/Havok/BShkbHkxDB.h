#pragma once

#include <Misc/BSFixedString.h>

// Fallout 4 BShkbHkxDB::ProjectDBData: 0x198. The variable map is a
// BSTHashMap<BSFixedString, int> (BSTScatterTable).
struct BShkbHkxDB
{
    struct VariableEntry
    {
        BSFixedString name;   // 00
        int32_t index;        // 08
        VariableEntry* next;  // 10, null for an empty slot
    };
    static_assert(sizeof(VariableEntry) == 0x18);

    struct VariableMap
    {
        uint64_t pad0;          // 00
        uint32_t pad8;          // 08
        uint32_t capacity;      // 0C
        uint32_t free;          // 10
        uint32_t good;          // 14
        const void* sentinel;   // 18
        uint64_t allocatorPad;  // 20
        VariableEntry* entries; // 28
    };
    static_assert(sizeof(VariableMap) == 0x30);

    virtual ~BShkbHkxDB();

    uint8_t pad8[0x78 - 0x8];
    VariableMap animationVariables; // 78 mNameToVariableID
    uint8_t padA8[0x198 - 0xA8];
};

static_assert(offsetof(BShkbHkxDB, animationVariables) == 0x78);
static_assert(sizeof(BShkbHkxDB) == 0x198);
