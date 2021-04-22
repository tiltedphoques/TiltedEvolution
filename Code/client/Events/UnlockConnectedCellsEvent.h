#pragma once

struct TESObjectREFR;

struct UnlockConnectedCellsEvent
{
    UnlockConnectedCellsEvent(TESObjectREFR* apDoor, bool abIsUnlocked) : pDoor(apDoor), bIsUnlocked(abIsUnlocked)
    {
    }

    TESObjectREFR* pDoor;
    float bIsUnlocked;
};
