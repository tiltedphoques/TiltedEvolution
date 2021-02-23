#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/AnimationVariables.h>

struct MovementComponent
{
    uint64_t Tick;
    glm::vec3 Position;
    glm::vec3 Rotation;
    AnimationVariables Variables;
    float Direction;

    bool Sent;
};
