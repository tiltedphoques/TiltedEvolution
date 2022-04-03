#pragma once

#include <Forms/TESForm.h>
#include <Components/TESTexture.h>
#include <Components/TESModel.h>

class TESWeather;
class TESGlobal;

struct WeatherEntry
{
    TESWeather* pWeather;
    uint32_t uiChance;
    TESGlobal* pChanceVar;
};
static_assert(sizeof(WeatherEntry) == 0x18);

struct TESWeatherList : GameList<WeatherEntry>
{
    // typedef EffectArchetypes::ArchetypeFlag TESWeatherList::WarningStatus;
};

class TESClimate : public TESForm
{
  public:
    virtual ~TESClimate() = 0;

    struct Timing
    {
      public:
        enum class MoonPhaseLength
        {
            kPhaseLengthMask = 0x3F,

            kNone = 0,
            kMasser = 1 << 6,
            kSecunda = 1 << 7
        };

        struct Interval
        {
            uint8_t begin;
            uint8_t end;
        };
        static_assert(sizeof(Interval) == 0x2);

        Interval sunrise;
        Interval sunset;
        uint8_t volatility;
        uint8_t moonPhaseLength;
        uint8_t unk6;
        uint8_t unk7;
    };
    static_assert(sizeof(Timing) == 0x8);

    TESModel mNightSky;
    TESWeatherList weatherList;
    TESTexture txSkyObjects[2];
    Timing timing;
};

static_assert(sizeof(TESClimate) == 0x80);
