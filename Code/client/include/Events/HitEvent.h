#pragma once

struct TESObjectREFR;

struct HitEvent
{
    HitEvent(Actor* aHittee, float damage, Actor* aHitter) 
        : Hittee(aHittee), Damage(damage), Hitter(aHitter)
    {}

    Actor* Hittee;
    float Damage;
    Actor* Hitter;
};
