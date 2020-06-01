#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Misc/BSFixedString.h>

#pragma pack(push, 1) // This structure is packed in havok 

struct hkHashTable
{
    struct Entry
    {
        BSFixedString key;
        int32_t value;
        uint32_t padC;
        Entry* next;
    };

    uint32_t bucketCount; // 0
    uint8_t pad4[0xC - 0x4]; // 4
    void* end; // C
    uint8_t pad[0x1C - 0x14];
    Entry* buckets;  // 1C
};

static_assert(offsetof(hkHashTable, end) == 0xC);
static_assert(offsetof(hkHashTable, buckets) == 0x1C);

#pragma pack(pop)

#endif
