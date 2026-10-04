#pragma once

struct NEW_REFR_DATA;
struct TESObjectCELL;
struct TESWorldSpace;
struct NiPoint3;
struct TESForm;
struct TESFaction;
struct Actor;
struct ImageSpaceModifierInstance;

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

#if defined(TP_FALLOUT4)
    uint8_t pad[0x18];
    GridCellArray* cells;
    uint8_t pad20[0x44 - 0x20];
    int32_t currentGridX;
    int32_t currentGridY;
    int32_t centerGridX;
    int32_t centerGridY;
    TESObjectCELL* interiorCell;
    TESObjectCELL** interiorBuffer;
    TESObjectCELL** exteriorBuffer;
    uint8_t pad70[0xA0 - 0x70];
    GameValueList<NiPointer<ImageSpaceModifierInstance>> activeImageSpaceModifiers;
};

static_assert(offsetof(TES, cells) == 0x18);
static_assert(offsetof(TES, interiorCell) == 0x58);
static_assert(offsetof(TES, exteriorBuffer) == 0x68);
static_assert(offsetof(TES, activeImageSpaceModifiers) == 0xA0);

#else
    uint8_t pad[0x78];
    GridCellArray* cells;

    uint8_t pad80[0xB0 - 0x80];
    int32_t centerGridX;
    int32_t centerGridY;
    int32_t currentGridX;
    int32_t currentGridY;
    TESObjectCELL* interiorCell;
    TESObjectCELL** interiorBuffer;
    TESObjectCELL** exteriorBuffer;
    uint8_t padD8[0x108 - 0xD8];
    GameValueList<NiPointer<ImageSpaceModifierInstance>> activeImageSpaceModifiers;
};

static_assert(offsetof(TES, cells) == 0x78);
static_assert(offsetof(TES, interiorCell) == 0xC0);
static_assert(offsetof(TES, exteriorBuffer) == 0xD0);
static_assert(offsetof(TES, activeImageSpaceModifiers) == 0x108);

#endif

struct ProcessLists
{
    static ProcessLists* Get() noexcept;

#if defined(TP_FALLOUT4)
    uint8_t pad00[0x20];
    int32_t numberHighActors;
    uint8_t pad24[0x32 - 0x24];
    bool bProcessHigh;
    bool bProcessLow;
    bool bProcessMHigh;
    bool bProcessMLow;
    bool bProcessSchedule;
    uint8_t pad37[0x40 - 0x37];
#else
    uint8_t pad00[0x8];

    bool bProcessHigh;
    bool bProcessLow;
    bool bProcessMHigh;
    bool bProcessMLow;
    bool bProcessSchedule;
    uint8_t padD[0x3];
    int32_t numberHighActors;
    uint8_t pad14[0x30 - 0x14];
#endif
    GameArray<uint32_t> highActorHandleArray;
    GameArray<uint32_t> lowActorHandleArray;
    GameArray<uint32_t> middleHighActorHandleArray;
    GameArray<uint32_t> middleLowActorHandleArray;
    GameArray<uint32_t>* actorBuckets[4];
};

#if defined(TP_FALLOUT4)
static_assert(offsetof(ProcessLists, highActorHandleArray) == 0x40);
static_assert(offsetof(ProcessLists, actorBuckets) == 0xA0);
#else
static_assert(offsetof(ProcessLists, highActorHandleArray) == 0x30);
static_assert(offsetof(ProcessLists, actorBuckets) == 0x90);
#endif

struct Mod
{
#if defined(TP_FALLOUT4)
    uint8_t pad0[0x70];
    char filename[MAX_PATH];
    uint8_t pad174[0x334 - 0x174];
    uint32_t flags;
    uint8_t pad338[0x370 - 0x338];
    uint8_t standardId;
    uint8_t padStandardId;
    uint16_t liteId;
#else
    uint8_t pad0[0x58];
    char filename[104];
    uint8_t pad60[0x438 - 0xC0];
    uint32_t flags;
    uint8_t pad43C[0x478 - 0x43C];
    uint8_t standardId;
    uint8_t padStandardId;
    uint16_t liteId;

#endif

    [[nodiscard]] bool IsLoaded() const noexcept { return standardId != 0xFF; }

    [[nodiscard]] bool IsLite() const noexcept { return ((flags >> 9) & 1) != 0; }

    [[nodiscard]] uint16_t GetId() const noexcept { return IsLite() ? liteId : standardId; }

    [[nodiscard]] uint32_t GetFormId(uint32_t aBaseId) const noexcept;
};

#if defined(TP_FALLOUT4)
static_assert(offsetof(Mod, filename) == 0x70);
static_assert(offsetof(Mod, standardId) == 0x370);
static_assert(offsetof(Mod, liteId) == 0x372);
#else
static_assert(offsetof(Mod, filename) == 0x58);
static_assert(offsetof(Mod, standardId) == 0x478);
static_assert(offsetof(Mod, liteId) == 0x47A);
#endif

struct ModManager
{
    static ModManager* Get() noexcept;

    uint32_t Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter) noexcept;
    Mod* GetByName(const char* acpName) const noexcept;
    TESObjectCELL* GetCellFromCoordinates(int32_t aX, int32_t aY, TESWorldSpace* aWorldSpace, bool aSpawnCell) noexcept;

#if defined(TP_FALLOUT4)
    uint8_t pad0[0x1B8];
    GameArray<TESFaction*> factions;
    uint8_t pad1D0[0x7E8 - 0x1D0];
    GameArray<TESQuest*> quests;
    uint8_t pad800[0xFB0 - 0x800];
    GameList<Mod> mods;
#else
    // Form arrays start at 0x10 and are indexed by FormType, 0x18 bytes each.
    uint8_t pad0[0x118];
    GameArray<TESFaction*> factions;
    uint8_t pad130[0x748 - 0x130];
    GameArray<TESQuest*> quests;
    uint8_t pad760[0xD60 - 0x760];
    GameList<Mod> mods;
#endif
};

#if defined(TP_FALLOUT4)
static_assert(offsetof(ModManager, factions) == 0x1B8);
static_assert(offsetof(ModManager, quests) == 0x7E8);
static_assert(offsetof(ModManager, mods) == 0xFB0);
#else
static_assert(offsetof(ModManager, factions) == 0x118);
static_assert(offsetof(ModManager, quests) == 0x748);
static_assert(offsetof(ModManager, mods) == 0xD60);
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
