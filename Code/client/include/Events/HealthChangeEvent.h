#pragma once

struct TESObjectREFR;

struct HealthChangeEvent
{
    HealthChangeEvent(Actor* aHittee, float deltaHealth, Actor* aHitter) 
        : Hittee(aHittee), DeltaHealth(deltaHealth), Hitter(aHitter)
    {}

    Actor* Hittee;
    float DeltaHealth;
    Actor* Hitter;
};
