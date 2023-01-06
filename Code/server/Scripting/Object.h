#pragma once

#include <Scripting/EntityHandle.h>

#include <World.h>
#include <Components.h>

#include <Structs/LockData.h>

namespace Script
{

struct Object : EntityHandle
{
    Object(entt::entity aEntity, World& aWorld);

    const LockData& GetLockData();

    void Lock(uint8_t aLockLevel);
    void Unlock();

    Object& operator=(const Object& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }
};

}
