#pragma once

struct Actor;

struct MagicSyncEvent
{
    MagicSyncEvent(Actor* apActor) : pActor(apActor)
    {
    }

    Actor* pActor;
};
