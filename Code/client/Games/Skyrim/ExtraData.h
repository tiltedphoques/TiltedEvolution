#pragma once

enum class ExtraData : uint32_t
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
    EncounterZone = 0x70,
    AliasInstanceArray = 0x88,
    OutfitItem = 0x8E,
    TextDisplayData = 0x99,
    Enchantment = 0x9B,
    Soul = 0x9C,
};
