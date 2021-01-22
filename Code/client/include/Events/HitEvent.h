#pragma once

struct TESObjectREFR;

struct HitEvent
{
    HitEvent(Actor* aHit, float damage, Actor* aHitter) 
        : Hit(aHit), Damage(damage)
        , Hitter(aHitter)
    {}

    Actor* Hit;
    float Damage;
    Actor* Hitter;
};
