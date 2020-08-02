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

    uint32_t unk8;
    uint32_t unkC;
    uint32_t dimension;
    TESObjectCELL** arr;
};

#if TP_PLATFORM_64
static_assert(offsetof(GridCellArray, arr) == 0x18);
#else
static_assert(offsetof(GridCellArray, arr) == 0x10);
#endif

struct TES
{
    static TES* Get() noexcept;

#if TP_SKYRIM64
    uint8_t pad[0x78];
#elif TP_FALLOUT4
    uint8_t pad[0x18];
#endif
    GridCellArray* cells;

#if TP_FALLOUT4
    uint8_t pad20[0x58 - 0x20];
#elif TP_SKYRIM64
    uint8_t pad80[0xC0 - 0x80];
#endif
    TESObjectCELL* interiorCell;
};

#if TP_SKYRIM64
static_assert(offsetof(TES, cells) == 0x78);
static_assert(offsetof(TES, interiorCell) == 0xC0);
#elif TP_FALLOUT4
static_assert(offsetof(TES, cells) == 0x18);
static_assert(offsetof(TES, interiorCell) == 0x58);
#endif

struct ActorHolder
{
    static ActorHolder* Get() noexcept;

#if TP_SKYRIM
    uint8_t pad0[0x30];
#else
    uint8_t pad0[0x40];
#endif

    GameArray<uint32_t> actorRefs;
};

#if TP_SKYRIM64
static_assert(offsetof(ActorHolder, actorRefs) == 0x30);
#elif TP_FALLOUT4
static_assert(offsetof(ActorHolder, actorRefs) == 0x40);
#endif

struct Mod
{
#if TP_FALLOUT4
    uint8_t pad0[0x70];
#elif TP_SKYRIM64
    uint8_t pad0[0x58];
#endif

    char filename[104];

#if TP_FALLOUT4
    uint8_t pad78[0x334 - 0xD8];
#elif TP_SKYRIM64
    uint8_t pad60[0x438 - 0xC0];
#endif

    uint32_t flags;

#if TP_FALLOUT4
    uint8_t pad338[0x370 - 0x338];
#elif TP_SKYRIM64
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

#if TP_SKYRIM64
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

#if TP_FALLOUT4
    uint8_t pad0[0xFB0];
#elif TP_SKYRIM64
    uint8_t pad0[0xD60];
#endif

    GameList<Mod> mods;
};

#if TP_SKYRIM64
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

#if TP_PLATFORM_32
    uint8_t unk0[0x10C];
#else
    uint8_t unk0[0x118];
#endif
    Entry head;
};

