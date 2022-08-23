#pragma once

struct NEW_REFR_DATA;
struct TESObjectCELL;
struct TESWorldSpace;
struct NiPoint3;
struct TESForm;
struct Actor;

struct GridCellArray
{
    virtual ~GridCellArray();

    virtual void UnloadAll();
    virtual void sub_02();
    virtual void SetCenter(int aX, int aY);
    virtual void ProcessDeltaChange(int aOffsetX, int aOffsetY);
    virtual void UnloadOffset(int aOffsetX, int aOffsetY);
    virtual void LoadOffset(int aOffsetX, int aOffsetY);
    virtual void MoveCell(int aFromX, int aFromY, int aToX, int aToY);
    virtual void SwapCells(int aFirstX, int aFirstY, int aSecondX, int aSecondY);

    uint32_t unk8;
    uint32_t unkC;
    uint32_t dimension;
    TESObjectCELL** arr;
};

static_assert(offsetof(GridCellArray, arr) == 0x18);

struct TES
{
    static TES* Get() noexcept;

#if TP_SKYRIM
    uint8_t pad[0x78];
#elif TP_FALLOUT4
    uint8_t pad[0x18];
#endif
    GridCellArray* cells;

#if TP_FALLOUT4
    uint8_t pad20[0x48 - 0x20];
#elif TP_SKYRIM
    uint8_t pad80[0xB0 - 0x80];
#endif
    int32_t centerGridX;
    int32_t centerGridY;
    int32_t currentGridX;
    int32_t currentGridY;
    TESObjectCELL* interiorCell;
};

#if TP_SKYRIM
static_assert(offsetof(TES, cells) == 0x78);
static_assert(offsetof(TES, interiorCell) == 0xC0);
#elif TP_FALLOUT4
static_assert(offsetof(TES, cells) == 0x18);
static_assert(offsetof(TES, interiorCell) == 0x58);
#endif

struct ProcessLists
{
    static ProcessLists* Get() noexcept;

#if TP_SKYRIM64
    uint8_t pad00[0x8];
#elif TP_FALLOUT4
    uint8_t pad00[0x32];
#endif

    bool bProcessHigh;
    bool bProcessLow;
    bool bProcessMHigh;
    bool bProcessMLow;
    bool bProcessSchedule;
#if TP_SKYRIM64
    uint8_t padD[0x3];
    int32_t numberHighActors;
    uint8_t pad14[0x30 - 0x14];
#elif TP_FALLOUT4
    uint8_t pad37[0x40 - 0x37];
#endif
    GameArray<uint32_t> highActorHandleArray;
    GameArray<uint32_t> lowActorHandleArray;
    GameArray<uint32_t> middleHighActorHandleArray;
    GameArray<uint32_t> middleLowActorHandleArray;
    GameArray<uint32_t>* actorBuckets[4];
};

#if TP_SKYRIM64
static_assert(offsetof(ProcessLists, highActorHandleArray) == 0x30);
static_assert(offsetof(ProcessLists, actorBuckets) == 0x90);
#elif TP_FALLOUT4
static_assert(offsetof(ProcessLists, bProcessHigh) == 0x32);
static_assert(offsetof(ProcessLists, bProcessSchedule) == 0x36);
static_assert(offsetof(ProcessLists, highActorHandleArray) == 0x40);
#endif

struct Mod
{
#if TP_FALLOUT4
    uint8_t pad0[0x70];
#elif TP_SKYRIM
    uint8_t pad0[0x58];
#endif

    char filename[104];

#if TP_FALLOUT4
    uint8_t pad78[0x334 - 0xD8];
#elif TP_SKYRIM
    uint8_t pad60[0x438 - 0xC0];
#endif

    uint32_t flags;

#if TP_FALLOUT4
    uint8_t pad338[0x370 - 0x338];
#elif TP_SKYRIM
    uint8_t pad43C[0x478 - 0x43C];
#endif

    uint8_t standardId;
    uint8_t padStandardId;
    uint16_t liteId;

    [[nodiscard]] bool IsLoaded() const noexcept
    {
        return standardId != 0xFF;
    }
    
    [[nodiscard]] bool IsLite() const noexcept
    {
        return ((flags >> 9) & 1) != 0;
    }

    [[nodiscard]] uint16_t GetId() const noexcept
    {
        return IsLite() ? liteId : standardId;
    }
    
    [[nodiscard]] uint32_t GetFormId(uint32_t aBaseId) const noexcept;
};

#if TP_SKYRIM
static_assert(offsetof(Mod, filename) == 0x58);
static_assert(offsetof(Mod, standardId) == 0x478);
static_assert(offsetof(Mod, liteId) == 0x47A);
#elif TP_FALLOUT4
static_assert(offsetof(Mod, filename) == 0x70);
static_assert(offsetof(Mod, standardId) == 0x370);
static_assert(offsetof(Mod, liteId) == 0x372);
#endif

struct ModManager
{
    static ModManager* Get() noexcept;

    uint32_t Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter) noexcept;
    Mod* GetByName(const char* acpName) const noexcept;
    TESObjectCELL* GetCellFromCoordinates(int32_t aX, int32_t aY, TESWorldSpace* aWorldSpace, bool aSpawnCell) noexcept;

#if TP_SKYRIM64
    uint8_t pad0[0x748];
#elif TP_FALLOUT4
    uint8_t pad0[0x7E8];
#endif

    GameArray<TESQuest*> quests;

#if TP_SKYRIM64
    uint8_t pad760[0xD60 - 0x760];
#elif TP_FALLOUT4
    uint8_t pad800[0xFB0 - 0x800];
#endif

    GameList<Mod> mods;
};

#if TP_SKYRIM
static_assert(offsetof(ModManager, mods) == 0xD60);
#elif TP_FALLOUT4
static_assert(offsetof(ModManager, mods) == 0xFB0);
#endif

struct Setting
{
    void* unk0;
    uint64_t data;
    const char* name;
};

struct INISettingCollection
{
    static INISettingCollection* Get() noexcept;

    struct Entry
    {
        Setting* setting;
        Entry* next;
    };

    Setting* GetSetting(const char* acpName) noexcept;

    uint8_t unk0[0x118];
    Entry head;
};

