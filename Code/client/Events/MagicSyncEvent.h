#pragma once

struct Actor;

struct MagicSyncEvent
{
    MagicSyncEvent(Actor* apActor, bool aFlag) : pActor(apActor), Flag(aFlag)
    {
    }

    Actor* pActor;
    bool Flag;
};
