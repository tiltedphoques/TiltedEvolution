#pragma once

struct TESObjectREFR;

struct HealthChangeEvent
{
    HealthChangeEvent(Actor* aHittee, float deltaHealth) 
        : Hittee(aHittee), DeltaHealth(deltaHealth)
    {}

    Actor* Hittee;
    float DeltaHealth;
};
