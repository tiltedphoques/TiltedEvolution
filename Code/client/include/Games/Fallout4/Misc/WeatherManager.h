#pragma once

struct WeatherManager
{
    static WeatherManager* Get();

    uint8_t pad0[0x18];
};
