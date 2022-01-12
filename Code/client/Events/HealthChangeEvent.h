#pragma once

struct TESObjectREFR;

struct HealthChangeEvent
{
    HealthChangeEvent(uint32_t aHitteeId, float aDeltaHealth) 
        : HitteeId(aHitteeId), DeltaHealth(aDeltaHealth)
    {}

    uint32_t HitteeId;
    float DeltaHealth;
};
