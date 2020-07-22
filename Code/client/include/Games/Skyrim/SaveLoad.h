#pragma once

#if defined(TP_SKYRIM)

#pragma pack(push, 1)

struct BGSSaveLoadManager
{
    virtual ~BGSSaveLoadManager();

    struct SaveData
    {
        void* unk0; // maybe vtable
        uint32_t someCounter; // 0x8
        uint32_t unkC;
        void* unk10; // 0x10
        void* someFunction; // 0x18

        uint32_t unk58; // 0x58

        
        uint32_t flags; // 0x340

        char* saveName; // 0xBB0
    };

    void Save(SaveData* apData);

    struct Struct330
    {
        void* unk0; // 0
        void* data; // 8
    };

    uint8_t pad8[0x330 - 0x8];
    Struct330* struct330; // 330
};

#if TP_PLATFORM_32
static_assert(offsetof(BGSSaveLoadManager::SaveData, someFunction) == 0x10);
#else
static_assert(offsetof(BGSSaveLoadManager::SaveData, someFunction) == 0x18);
#endif

struct BGSSaveFormBuffer
{
    BGSSaveFormBuffer();
    virtual ~BGSSaveFormBuffer() {}

    void WriteId(uint32_t aId) noexcept;

    char* buffer; // 4 - 8
    uint32_t capacity; // 8 - 10
    uint32_t position; // C - 14
    uint8_t formId[3]; // 10 - 18 For some reason the flags start at 0x13 on oldrim
    uint32_t changeFlags; // 13 - 1B

    uint8_t pad[0x100]; // Ensure we have enough space as we don't know the exact size
};

#if TP_PLATFORM_32
static_assert(offsetof(BGSSaveFormBuffer, formId) == 0x10);
static_assert(offsetof(BGSSaveFormBuffer, changeFlags) == 0x13);
#else
static_assert(offsetof(BGSSaveFormBuffer, formId) == 0x18);
static_assert(offsetof(BGSSaveFormBuffer, changeFlags) == 0x1B);
#endif

#if TP_PLATFORM_32
struct BGSLoadFormBuffer
{
    BGSLoadFormBuffer(uint32_t aChangeFlags);
    virtual ~BGSLoadFormBuffer() {}

    void SetSize(const uint32_t aSize) noexcept
    {
        capacity = size1 = size2 = aSize;
    }

    const char* buffer; // 4
    void* unk8; // 8
    uint32_t capacity; // C
    uint32_t size1; // 10
    uint32_t position; // 14
    uint32_t formId; // 18 
    uint32_t size2; // 1C - same as size
    void* unkPtr; // 20
    struct TESForm* form; // 24
    uint32_t changeFlags; // 28
    uint32_t maybeMoreFlags; // 2C ??????
    uint8_t unk30;
    uint8_t unk31;
    uint8_t unk32;
    uint8_t loadFlag;

    uint8_t pad[0x100]; // Ensure we have enough space as we don't know the exact size
};
#else
struct BGSLoadFormBuffer
{
    BGSLoadFormBuffer(uint32_t aChangeFlags);
    virtual ~BGSLoadFormBuffer() {}

    void SetSize(const uint32_t aSize) noexcept
    {
        capacity = size1 = size2 = aSize;
    }

    const char* buffer; // 8
    size_t capacity; // 10
    uint32_t unk18; // 18 - 0x459 ? version maybe
    int32_t unk1C; // 1C - set to -1
    uint32_t size1; // 20
    uint32_t position; // 24
    uint32_t formId; // 28
    uint32_t size2; // 2C
    uint32_t unk30; // 30 - usually 0
    uint32_t unk34; // 34 - 0 or 1 or 0xFFFF
    struct TESForm* form; // 38
    uint32_t changeFlags; // 40
    uint32_t maybeMoreFlags; // 44 ??????
    uint8_t unk48; // 48 some flags apparently
    uint8_t unk49;
    uint8_t unk4A;
    uint8_t loadFlag;
};
#endif

#if TP_PLATFORM_32
static_assert(offsetof(BGSLoadFormBuffer, changeFlags) == 0x28);
static_assert(offsetof(BGSLoadFormBuffer, loadFlag) == 0x33);
#else
static_assert(offsetof(BGSLoadFormBuffer, changeFlags) == 0x40);
static_assert(offsetof(BGSLoadFormBuffer, loadFlag) == 0x4B);
#endif

#pragma pack(pop)

#endif
