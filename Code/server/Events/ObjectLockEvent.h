#pragma once

#include <Structs/LockData.h>

struct ObjectLockEvent
{
    ObjectLockEvent(entt::entity aEntity, LockData aLock)
        : Entity(aEntity), Lock(aLock)
    {
    }

    entt::entity Entity;
    LockData Lock;
};
