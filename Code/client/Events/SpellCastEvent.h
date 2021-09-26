#pragma once

struct Actor;

struct SpellCastEvent
{
    SpellCastEvent(Actor* apActor)
        : pActor(apActor)
    {}

    Actor* pActor;
};
