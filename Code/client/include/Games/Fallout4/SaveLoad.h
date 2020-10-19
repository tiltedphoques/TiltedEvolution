#pragma once

#pragma pack(push, 1)

struct BGSSaveLoadManager
{
    virtual ~BGSSaveLoadManager();

    static BGSSaveLoadManager* Get() noexcept;

    uint8_t pad8[0x3C - 0x8];
    uint8_t someBool; // 3C - set to 1 skips write
};

static_assert(offsetof(BGSSaveLoadManager, someBool) == 0x3C);

struct BGSSaveFormBuffer
{
    BGSSaveFormBuffer();
    virtual ~BGSSaveFormBuffer() {}

    void WriteId(uint32_t aId) noexcept;

    char* buffer; // 4 - 8
    uint32_t capacity; // 8 - 10
    uint32_t position; // C - 14
    uint8_t formId24[3]; // 10 - 18 For some reason the flags start at 0x13 on oldrim
    uint32_t changeFlags; // 13 - 1B
    uint8_t pad[0x50];
};

/*static_assert(offsetof(BGSSaveFormBuffer, pad10) == 0x18);
static_assert(offsetof(BGSSaveFormBuffer, changeFlags) == 0x1B);*/

struct BGSLoadFormBuffer
{
    BGSLoadFormBuffer(uint32_t aChangeFlags);
    virtual ~BGSLoadFormBuffer() {}

    void SetSize(const uint32_t aSize) noexcept
    {
        capacity = size1 = aSize;
        size2 = 0;
    }

    const char* buffer; // 8
    struct ScrapHeap* scrapHeap; // 10
    uint32_t capacity; // 18
    uint32_t unk1C; // 1C - set to 1
    uint32_t size1; // 20
    uint32_t position; // 24
    uint32_t formId; // 28
    uint32_t size2; // 2C
    void* unk30; // 30
    struct TESForm* form; // 38
    uint32_t changeFlags; // 40
    uint32_t maybeMoreFlags; // 44 ??????
    uint8_t unk48; // 48 - some flags apparently
    uint8_t unk49;
    uint8_t unk4A;
    uint8_t loadFlag;
    uint8_t pad[0x50];
};

static_assert(offsetof(BGSLoadFormBuffer, changeFlags) == 0x40);

#pragma pack(pop)
