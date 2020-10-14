#pragma once

#if TP_FALLOUT4

struct WeatherManager
{
    static WeatherManager* Get();

    uint8_t pad0[0x18];
};

#endif
