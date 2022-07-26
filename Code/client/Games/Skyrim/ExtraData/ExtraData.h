#pragma once

#include <Games/Primitives.h>

struct TESObjectREFR;
struct TESForm;

enum class ExtraDataType : uint32_t
{
    None = 0,
    ContainerChanges = 0x15,
    Worn = 0x16,
    WornLeft = 0x17,
    ReferenceHandle = 0x1C,
    Count = 0x24,
    Health = 0x25,
    Charge = 0x28,
    Teleport = 0x2B,
    LeveledCreature = 0x2D,
    CannotWear = 0x3D,
    Poison = 0x3E,
    Faction = 0x5B,
    AliasInstanceArray = 0x88,
    OutfitItem = 0x8E,
    TextDisplayData = 0x99,
    Enchantment = 0x9B,
    Soul = 0x9C,
};

struct BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::None;

    virtual ~BSExtraData() = 0;
    virtual ExtraDataType GetType() const noexcept = 0;

    BSExtraData* next{};

#if TP_FALLOUT4
    uint16_t usFlags;
    ExtraDataType type;
#endif
};

#if TP_FALLOUT4
static_assert(offsetof(BSExtraData, type) == 0x12);
static_assert(sizeof(BSExtraData) == 0x18);
#endif

