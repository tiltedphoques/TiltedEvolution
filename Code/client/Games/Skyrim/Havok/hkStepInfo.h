#pragma once

#include <cmath>
#include <cstddef>

struct hkStepInfo
{
    bool UpdateDeltaTime(float aPhysicsDeltaTime, float aMovementDeltaTime = 0.f) noexcept
    {
        return TrySetDeltaTime(aPhysicsDeltaTime) || TrySetDeltaTime(aMovementDeltaTime) || TrySetDeltaTime(deltaTime);
    }

    float startTime;
    float endTime;
    float deltaTime;
    float invDeltaTime;

private:
    bool TrySetDeltaTime(float aDeltaTime) noexcept
    {
        // The native controller movement path also rejects steps <= 0.0001s
        if (!std::isfinite(aDeltaTime) || aDeltaTime <= 0.0001f)
            return false;

        deltaTime = aDeltaTime;
        invDeltaTime = 1.f / aDeltaTime;
        return true;
    }
};

static_assert(offsetof(hkStepInfo, deltaTime) == 0x8);
static_assert(offsetof(hkStepInfo, invDeltaTime) == 0xC);
static_assert(sizeof(hkStepInfo) == 0x10);
