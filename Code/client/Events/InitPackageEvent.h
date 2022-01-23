#pragma once

struct InitPackageEvent
{
    InitPackageEvent(uint32_t aActorId, uint32_t aPackageId)
        : ActorId(aActorId), PackageId(aPackageId)
    {
    }

    uint32_t ActorId;
    uint32_t PackageId;
};
