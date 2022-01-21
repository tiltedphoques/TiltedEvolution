#pragma once

struct MountEvent
{
    MountEvent(uint32_t aRiderID, uint32_t aMountID)
        : RiderID(aRiderID), MountID(aMountID)
    {}

    uint32_t RiderID;
    uint32_t MountID;
};
