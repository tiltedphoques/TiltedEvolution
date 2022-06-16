#pragma once

/**
 * @brief Dispatched whenever the mapmenu needs to update the player mapmarkers.
 */
struct PlayerSetWaypointEvent
{
    PlayerSetWaypointEvent(Vector3_NetQuantize aPosition)
    {
        Position = aPosition;
    }
	
    Vector3_NetQuantize Position;
};
