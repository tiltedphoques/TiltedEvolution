#pragma once

struct TESObjectREFR;

/**
* @brief Dispatched whenever the lock status on an object changes.
*/
struct LockChangeEvent
{
    LockChangeEvent(TESObjectREFR* apObject, bool aIsLocked, uint8_t aLockLevel) 
        : pObject(apObject), IsLocked(aIsLocked), LockLevel(aLockLevel)
    {}

    // TODO(cosideci): this shouldn't be a ptr, but a form id
    TESObjectREFR* pObject;
    bool IsLocked;
    uint8_t LockLevel;
};
