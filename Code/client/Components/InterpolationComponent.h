#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/AnimationVariables.h>

struct InterpolationComponent
{
    struct TimePoint
    {
        uint64_t Tick{};
        glm::vec3 Position{};
        glm::vec3 Rotation{};
        AnimationVariables Variables{};
        float Direction{};

        TimePoint() = default;
        TimePoint(const TimePoint&) = default;
        TimePoint& operator=(const TimePoint&) = default;
    };

    InterpolationComponent(std::chrono::steady_clock::time_point aPoint)
        : Last3DTimePoint(aPoint)
    {}

    std::chrono::steady_clock::time_point Last3DTimePoint;
    List<TimePoint> TimePoints;
    glm::vec3 Position;
};
