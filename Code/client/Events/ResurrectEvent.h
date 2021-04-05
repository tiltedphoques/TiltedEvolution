#pragma once

struct ResurrectEvent
{
    ResurrectEvent(Actor* apActor)
        : pActor(apActor)
    {}

    Actor* pActor;
};
