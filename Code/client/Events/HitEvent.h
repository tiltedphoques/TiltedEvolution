#pragma once

struct HitEvent
{
    HitEvent() = delete;
    HitEvent(uint32_t aHitterId, uint32_t aHitteeId) 
        : HitterId(aHitterId), HitteeId(aHitteeId)
    {
    }

    uint32_t HitterId;
    uint32_t HitteeId;
};
