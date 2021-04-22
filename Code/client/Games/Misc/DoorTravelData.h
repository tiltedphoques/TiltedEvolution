#pragma once

struct TESObjectCELL;
struct TESObjectREFR;

struct DoorTravelData
{
    void UnlockConnectedCells(TESObjectREFR* apDoor, bool abIsUnlocked) noexcept;

    TESObjectCELL* connectedCell;
    uint8_t pad8[0x20];
};

static_assert(sizeof(DoorTravelData) == 0x28);
