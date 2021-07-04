#pragma once

struct Actor;

struct MagicCaster
{
    void* FireProjectile() noexcept;

    uint8_t pad0[0xB8];
    Actor* pCastingActor;
};
