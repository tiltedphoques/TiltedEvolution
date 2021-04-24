#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/LockData.h>

struct LockComponent
{
    LockComponent(const LockData& aLockData) noexcept
        : CurrentLockData(aLockData)
    {}

    LockData CurrentLockData{};
};
