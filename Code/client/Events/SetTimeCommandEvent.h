#pragma once

struct SetTimeCommandEvent
{
    SetTimeCommandEvent() = delete;
    SetTimeCommandEvent(int32_t aHours, int32_t aMinutes)
        : Hours(aHours), Minutes(aMinutes)
    {
    }

    int32_t Hours;
    int32_t Minutes;
};
