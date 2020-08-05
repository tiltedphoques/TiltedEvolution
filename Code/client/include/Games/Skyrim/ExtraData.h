#pragma once

#ifdef TP_SKYRIM

enum class ExtraData : uint32_t
{
    ContainerChanges = 0x15,
    Worn = 0x16,
    WornLeft = 0x17,
    ReferenceHandle = 0x1C,
    Teleport = 0x2B,
    LeveledCreature = 0x2D,
    CannotWear = 0x3D,
    Faction = 0x5B,
    AliasInstanceArray = 0x88
};

#endif
