#pragma once

struct TESObjectREFR;

struct LockChangeEvent
{
    LockChangeEvent(TESObjectREFR* apObject, char aiIsLocked, uint8_t aiLockLevel) 
        : pObject(apObject), iIsLocked(aiIsLocked), iLockLevel(aiLockLevel)
    {}

    TESObjectREFR* pObject;
    uint8_t iIsLocked;
    uint8_t iLockLevel;
};
