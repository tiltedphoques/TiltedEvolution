#pragma once

/**
* @brief Dispatched when a spell casting is interrupted locally.
*/
struct InterruptCastEvent
{
    InterruptCastEvent(uint32_t aCasterFormID) : CasterFormID(aCasterFormID)
    {}

    uint32_t CasterFormID;
};

