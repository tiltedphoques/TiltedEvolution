#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct InterpolationComponent
{
    struct TimePoint
    {
        uint64_t Tick;
        Vector3<float> Position;
        Vector3<float> Rotation;
    };

    List<TimePoint> TimePoints;
};
