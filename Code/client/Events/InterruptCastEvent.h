#pragma once

/**
* @brief Dispatched when a spell casting is interrupted locally.
*/
struct InterruptCastEvent
{
    InterruptCastEvent(uint32_t aCasterFormID, int32_t aCastingSource)
        : CasterFormID(aCasterFormID), CastingSource(aCastingSource)
    {}

    uint32_t CasterFormID;
    int32_t CastingSource;
};

