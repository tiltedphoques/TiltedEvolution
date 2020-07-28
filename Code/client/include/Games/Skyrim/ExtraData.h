#pragma once

#ifdef TP_SKYRIM

#include <Games/Primitives.h>

struct TESObjectREFR;

enum class ExtraData : uint32_t
{
    ContainerChanges = 0x15,
    Worn = 0x16,
    WornLeft = 0x17,
    ReferenceHandle = 0x1C,
    Teleport = 0x2B,
    CannotWear = 0x3D,
    Faction = 0x5B,
    AliasInstanceArray = 0x88
};

struct BSExtraData
{
    virtual ~BSExtraData();
    virtual ExtraData GetType() const noexcept;

    BSExtraData* next;
};


struct BSExtraDataList
{
    bool Contains(ExtraData aType) const;
    void Set(ExtraData aType, bool aSet);

    bool Add(ExtraData aType, BSExtraData* apOther);
    bool Remove(ExtraData aType, BSExtraData* apOther);

    BSExtraData* GetByType(ExtraData type) const;
    BSExtraData* data;

    struct Bitfield
    {
        uint8_t data[0x18];
    };

    Bitfield* bitfield;
    BSRecursiveLock lock;
};

#endif
