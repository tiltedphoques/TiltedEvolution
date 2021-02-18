#pragma once

struct TESObjectREFR;

struct HealthChangeEvent
{
    HealthChangeEvent(Actor* apHittee, float aDeltaHealth) 
        : pHittee(apHittee), DeltaHealth(aDeltaHealth)
    {}

    Actor* pHittee;
    float DeltaHealth;
};
