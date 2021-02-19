#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/AnimationVariables.h>

struct MovementComponent
{
    uint64_t Tick;
    Vector3<float> Position;
    Vector3<float> Rotation;
    AnimationVariables Variables;
    float Direction;

    bool Sent;
};
