#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/AnimationVariables.h>
#include <Systems/PlaybackClock.h>

struct InterpolationComponent
{
    struct TimePoint
    {
        // The owner's capture tick
        uint64_t Tick{};
        glm::vec3 Position{};
        glm::vec3 Rotation{};
        AnimationVariables Variables{};
        float Direction{};

        TimePoint() = default;
    };

    List<TimePoint> TimePoints;
    // Last displayed position
    glm::vec3 Position;
    // Last displayed heading (rotation z)
    float Yaw{};

    PlaybackClock Clock;

    // Left over from correcting an extrapolation, decays over a few frames instead of popping the actor into place
    glm::vec3 PositionError{};
    bool IsExtrapolating{false};
    bool HasNewTimePoint{false};
    // The horse graph only runs its legs between moveStart and moveStop, see InterpolationSystem
    bool IsMountMoving{false};
    // Tick the horse's Speed first dropped below the stop threshold, 0 while it didn't
    uint64_t MountLowSpeedSince{0};
};
