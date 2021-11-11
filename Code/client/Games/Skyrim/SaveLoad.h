#pragma once

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

static_assert(offsetof(BGSSaveLoadManager::SaveData, someFunction) == 0x18);

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

static_assert(offsetof(BGSSaveFormBuffer, formId) == 0x18);
static_assert(offsetof(BGSSaveFormBuffer, changeFlags) == 0x1B);

struct BGSSaveLoadBuffer
{
    char* pBuffer;
};

static_assert(sizeof(BGSSaveLoadBuffer) == 0x8);

struct BGSSaveGameBuffer
{
    virtual ~BGSSaveGameBuffer();

    BGSSaveLoadBuffer Buffer;
    uint32_t iBufferSize;
    uint32_t iBufferPosition;
};

static_assert(sizeof(BGSSaveGameBuffer) == 0x18);

struct BGSChangeFlags
{
    int32_t iFlags;
};

static_assert(sizeof(BGSChangeFlags) == 0x4);

struct BGSSaveLoadFormInfo
{
    uint8_t cData;
};

static_assert(sizeof(BGSSaveLoadFormInfo) == 0x1);

struct BGSNumericIDIndex
{
    uint8_t cData1;
    uint8_t cData2;
    uint8_t cData3;
};

struct BGSSaveLoadFormHeader
{
    BGSNumericIDIndex FormIDIndex;
    BGSChangeFlags iChangeFlags;
    BGSSaveLoadFormInfo FormInfo;
    uint8_t cVersion;
};

static_assert(sizeof(BGSSaveLoadFormHeader) == 0x9);

struct BGSSaveFormBufferReal : BGSSaveGameBuffer
{
    BGSSaveLoadFormHeader Header;
    uint8_t pad21[0x7];
    struct TESForm* pForm;
};

static_assert(sizeof(BGSSaveFormBufferReal) == 0x30);

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

static_assert(offsetof(BGSLoadFormBuffer, changeFlags) == 0x40);
static_assert(offsetof(BGSLoadFormBuffer, loadFlag) == 0x4B);

// TODO: mostly copied from fallout 4, needs to be validated
struct __declspec(align(8)) BGSSaveLoadScrapBuffer
{
    char* pBuffer;
    struct ScrapHeap* pScrapHeap;
    uint32_t uiSize;
};

static_assert(sizeof(BGSSaveLoadScrapBuffer) == 0x18);

struct BGSLoadGameBuffer
{
    virtual ~BGSLoadGameBuffer();

    BGSSaveLoadScrapBuffer Buffer;
    uint32_t iBufferSize;
    uint32_t iBufferPosition;
};

static_assert(sizeof(BGSLoadGameBuffer) == 0x28);

struct __declspec(align(8)) BGSLoadFormData
{
    uint32_t iFormID;
    uint32_t uiDataSize;
    uint32_t uiUncompressedSize;
    TESForm* pForm;
    BGSChangeFlags iChangeFlags;
    BGSChangeFlags iOldChangeFlags;
    uint16_t usFlags;
    BGSSaveLoadFormInfo FormInfo;
    uint8_t cVersion;
};

//static_assert(sizeof(BGSLoadFormData) == 0x28);

struct BGSLoadFormBufferReal : BGSLoadGameBuffer, BGSLoadFormData
{
};

//static_assert(sizeof(BGSLoadFormBufferReal) == 0x50);

#pragma pack(pop)
