#pragma once

struct ActorState
{
    virtual ~ActorState();

    uint32_t flags1;
    uint32_t flags2;

    bool IsWeaponDrawn() const noexcept
    {
        return ((flags2 >> 1) & 7) >= 3;
    }
};
