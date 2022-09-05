#pragma once

struct Sky
{
    static Sky* Get() noexcept;

    virtual ~Sky();

    void SetWeather(TESWeather* apWeather) noexcept;
    void ForceWeather(TESWeather* apWeather) noexcept;

    TESWeather* GetWeather() const noexcept;

    uint8_t unk8[0x48 - 0x8];
    TESWeather* pCurrentWeather;
    uint8_t unk50[0x480 - 0x50];
};

static_assert(sizeof(Sky) == 0x480);
