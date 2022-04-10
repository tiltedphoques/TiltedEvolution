
#include "Shared/Sky/Sky.h"
#include "Shared/Sky/Moon.h"

#include <Services/SkyService.h>
#include <World.h>

static Sky* (*Sky_GetInstance)(){nullptr};
static void (*Sky_SetWeather)(Sky*, TESWeather*, bool, bool){nullptr};
static uint32_t (*BSRandom_UnsignedInt)(void*, uint32_t){nullptr};

Sky* Sky::GetInstance()
{
    return Sky_GetInstance();
}

void Sky::SetHideSky(bool abVal)
{
    if (abVal)
        uiFlags |= HIDE_SKY;
    else
        uiFlags &= ~HIDE_SKY;
}

void Sky::SetFastTravel(bool abOn)
{
    if (abOn)
        uiFlags |= FAST_TRAVEL;
    else
        uiFlags &= ~FAST_TRAVEL;
}

float Sky::GetCurrentWeatherTimeElapsed()
{
    float fCurrentGameHour = this->fCurrentGameHour;
    float fLastWeatherUpdate = this->fLastWeatherUpdate;
    if (fCurrentGameHour >= fLastWeatherUpdate)
        return (fCurrentGameHour + 0.0) - fLastWeatherUpdate;
    else
        return (fCurrentGameHour + 24.0) - fLastWeatherUpdate;
}

// if GetCurrentWeatherTimeElapsed > GetCurrentWeatherDuration then
//    select new random weather
float Sky::GetCurrentWeatherDuration()
{
    TESClimate* pCurrentClimate = this->pCurrentClimate;
    float duration = 1.f;
    if (!pCurrentClimate)
        return duration;
    TESWeather* pCurrentWeather = this->pCurrentWeather;
    if (!pCurrentWeather)
        return duration;

// in fallout4 there is a multiplier, which doesnt seem to be present in skyrim
#if 0
    constexpr float kVolatiliyMultiplier = 1.f;
    float vola = pCurrentClimate->timing.volatility * kVolatiliyMultiplier;
    if (vola <= 255.0)
    {
        if (vola < 0.0)
            vola = 0.0;
    }
    else
    {
        vola = 255.f;
    }
#endif
    float vola = static_cast<float>(pCurrentClimate->timing.volatility);

    // starting from fallout 4, the constant 32 is replaced by fWeatherDurationMax
    // and the constant 1.f is replaced by fWeatherDurationMin

    // constexpr auto f = 1.f / 255.f = 0.0039215689;
    duration = (((23.f - 1.f) * (255.f - vola)) * 0.0039215689) + 1.f;
    return duration < 0.1f ? 0.1f : duration;
}

//(23.f - 1.f) * (255.0f) * 0.0039215689
// constexpr float kWeatherDuration = (((23.f - 1.f) * (255.0f)) * 0.0039215689) + 1.f;

void Sky::SetWeatherExternal(TESWeather* newWeather, bool overrideWeather, bool updateLastWeather)
{
    Sky_SetWeather(this, newWeather, overrideWeather, updateLastWeather);
}

// sub called from Sky::Update->Sky::UpdateWeather
uint32_t Hook_BSRandom_UnsignedInt(void* apGenerator, uint32_t aMax)
{
    if (SkyService::UseOnlineTick())
    {
        return World::Get().ctx<SkyService>().GetWeatherSeed();
    }

    return BSRandom_UnsignedInt(apGenerator, aMax);
}

static TiltedPhoques::Initializer s_SkyInit([]() {
    const VersionDbPtr<uint8_t> s_setWeather(26241);
    const VersionDbPtr<uint8_t> s_getInstance(13878);
    Sky_SetWeather = static_cast<decltype(Sky_SetWeather)>(s_setWeather.GetPtr());
    Sky_GetInstance = static_cast<decltype(Sky_GetInstance)>(s_getInstance.GetPtr());

    // TODO(Force): examine if this is the only required location.

    //Sky::UpdateWeather
    TiltedPhoques::SwapCall(0x1402C226F, BSRandom_UnsignedInt, &Hook_BSRandom_UnsignedInt);

    //Sky::UpdateSounds
    TiltedPhoques::SwapCall(0x1403CB4A3, BSRandom_UnsignedInt, &Hook_BSRandom_UnsignedInt);
});
