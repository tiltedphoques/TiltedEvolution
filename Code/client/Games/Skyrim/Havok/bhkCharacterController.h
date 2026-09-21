#pragma once

#include <Games/Primitives.h>
#include <Havok/hkStepInfo.h>

struct bhkCharacterController : NiRefObject
{
    bool UpdateStepTiming(float aMovementDeltaTime = 0.f) noexcept;

    uint8_t pad10[0x80 - 0x10];
    hkStepInfo stepInfo;
};

static_assert(offsetof(bhkCharacterController, stepInfo) == 0x80);
static_assert(offsetof(bhkCharacterController, stepInfo) + offsetof(hkStepInfo, deltaTime) == 0x88);
static_assert(offsetof(bhkCharacterController, stepInfo) + offsetof(hkStepInfo, invDeltaTime) == 0x8C);
