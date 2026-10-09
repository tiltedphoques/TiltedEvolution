#pragma once

struct TESWeather;

// Fallout 4 Sky: 0x480.
struct Sky
{
    static Sky* Get() noexcept;

    inline static bool s_shouldUpdateWeather = true;

    virtual ~Sky();

    void SetWeather(TESWeather* apWeather) noexcept;
    void ForceWeather(TESWeather* apWeather) noexcept;
    void ReleaseWeatherOverride() noexcept;
    void ResetWeather() noexcept;

    TESWeather* GetWeather() const noexcept;

    uint8_t unk8[0x48 - 0x8];
    TESWeather* pCurrentWeather; // 0x48
    uint8_t unk50[0x480 - 0x50];
};

static_assert(offsetof(Sky, pCurrentWeather) == 0x48);
static_assert(sizeof(Sky) == 0x480);
