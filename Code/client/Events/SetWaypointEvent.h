#pragma once

/**
 * @brief Dispatched whenever the player sets a waypoint
 */
struct SetWaypointEvent
{
    SetWaypointEvent(Vector3_NetQuantize aPosition, uint32_t aWorldSpaceFormID) 
        : Position(aPosition), WorldSpaceFormID(aWorldSpaceFormID)
    {}

    Vector3_NetQuantize Position;
    uint32_t WorldSpaceFormID;
};
