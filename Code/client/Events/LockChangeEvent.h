#pragma once

struct TESObjectREFR;

struct LockChangeEvent
{
    LockChangeEvent(TESObjectREFR* apObject, bool aIsLocked, uint8_t aLockLevel) 
        : pObject(apObject), IsLocked(aIsLocked), LockLevel(aLockLevel)
    {}

    TESObjectREFR* pObject;
    bool IsLocked;
    uint8_t LockLevel;
};
