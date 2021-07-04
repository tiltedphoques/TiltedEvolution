#pragma once

struct Actor;

struct FireProjectileEvent
{
    FireProjectileEvent(Actor* apCastingActor) : pCastingActor(apCastingActor)
    {
    }

    Actor* pCastingActor;
};
