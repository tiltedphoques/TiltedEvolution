#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Misc/IMovementState.h>

struct ActorState : IMovementState
{
    virtual ~ActorState();

    uint32_t flags1;
    uint32_t flags2;

    bool IsWeaponDrawn() const noexcept
    {
        return (flags2 >> 5 & 7) >= 3;
    }
};

#endif
