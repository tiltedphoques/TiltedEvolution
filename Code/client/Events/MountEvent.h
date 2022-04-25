#pragma once

/**
* @brief Dispatched whenever a local actor mounts another actor (i.e. a horse or dragon).
*/
struct MountEvent
{
    MountEvent(uint32_t aRiderID, uint32_t aMountID)
        : RiderID(aRiderID), MountID(aMountID)
    {}

    uint32_t RiderID;
    uint32_t MountID;
};
