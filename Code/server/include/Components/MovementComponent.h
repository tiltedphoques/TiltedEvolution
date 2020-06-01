#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct MovementComponent
{
    uint64_t Tick;
    Vector3<float> Position;
    Vector3<float> Rotation;
    float Speed;
    float Direction;

    bool Sent;
};
