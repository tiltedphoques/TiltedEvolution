#pragma once

#if TP_SKYRIM

#pragma pack(push, 1) // This structure is packed in havok 

struct hkHashTable
{
    struct Entry
    {
        const char* key;
        int32_t value;
#if TP_PLATFORM_64
        uint32_t padC;
#endif
        Entry* next;
    };

    uint32_t bucketCount; // 0
#if TP_PLATFORM_64
    uint8_t pad4[0xC - 0x4]; // 4
#endif
    void* end; // C
    uint8_t pad[0x1C - 0x14];
    Entry* buckets;  // 1C
};

static_assert(offsetof(hkHashTable, end) == 0xC);
static_assert(offsetof(hkHashTable, buckets) == 0x1C);

#pragma pack(pop)

#endif
