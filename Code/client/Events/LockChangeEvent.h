#pragma once

/**
* @brief Dispatched whenever the lock status on an object changes.
*/
struct LockChangeEvent
{
    LockChangeEvent(uint32_t aFormId, bool aIsLocked, uint8_t aLockLevel) 
        : FormId(aFormId), IsLocked(aIsLocked), LockLevel(aLockLevel)
    {}

    uint32_t FormId;
    bool IsLocked;
    uint8_t LockLevel;
};
