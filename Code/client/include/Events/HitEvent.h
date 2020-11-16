#pragma once

struct TESObjectREFR;

struct HitEvent
{
    HitEvent(TESObjectREFR* aHit, TESObjectREFR* aHitter) 
        : Hit(aHit)
        , Hitter(aHitter)
    {}

    TESObjectREFR* Hit;
    TESObjectREFR* Hitter;
};
