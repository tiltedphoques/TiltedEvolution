#pragma once

/**
* @brief Dispatched on every frame.
* 
* All entities are safe to access during this event.
*/
struct UpdateEvent
{
    explicit UpdateEvent(const double aDelta)
        : Delta(aDelta)
    {}

    double Delta;
};
