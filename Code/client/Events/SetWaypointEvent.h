#pragma once

/**
 * @brief Dispatched whenever the player sets a waypoint
 */
struct SetWaypointEvent
{
    SetWaypointEvent(Vector3_NetQuantize aPosition) 
        : Position(aPosition)
    {}

    Vector3_NetQuantize Position;
};
