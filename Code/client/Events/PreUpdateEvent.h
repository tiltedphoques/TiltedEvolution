#pragma once

/**
* Register to this event only if you do not use entities,
* this is used to generate the list of valid entities.
*/
struct PreUpdateEvent
{
    explicit PreUpdateEvent(const double aDelta)
        : Delta(aDelta)
    {}

    PreUpdateEvent& operator=(const PreUpdateEvent& acRhs) = default;

    double Delta;
};
