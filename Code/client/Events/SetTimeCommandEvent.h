#pragma once

struct SetTimeCommandEvent
{
    SetTimeCommandEvent() = delete;
    SetTimeCommandEvent(int32_t aHours, int32_t aMinutes) : Hours(aHours), Minutes(aMinutes)
    {
    }

    uint8_t Hours;
    uint8_t Minutes;
};
