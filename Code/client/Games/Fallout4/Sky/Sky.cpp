#include "Sky.h"

Sky* Sky::Get() noexcept
{
    using TGetInstance = Sky*();
    static VersionDbPtr<TGetInstance> getInstance(2192448);
    return getInstance.Get()();
}

void Sky::SetWeather(TESWeather* apWeather) noexcept
{
    TP_THIS_FUNCTION(TSetWeather, void, Sky, TESWeather* apWeather, bool abOverride, bool abAccelerate);
    static VersionDbPtr<TSetWeather> setWeather(2208859);
    TiltedPhoques::ThisCall(setWeather, this, apWeather, true, true);
}

void Sky::ForceWeather(TESWeather* apWeather) noexcept
{
    TP_THIS_FUNCTION(TForceWeather, void, Sky, TESWeather* apWeather, bool abOverride);
    static VersionDbPtr<TForceWeather> forceWeather(2208861);
    TiltedPhoques::ThisCall(forceWeather, this, apWeather, true);
}

void Sky::ReleaseWeatherOverride() noexcept
{
    TP_THIS_FUNCTION(TReleaseWeatherOverride, void, Sky);
    static VersionDbPtr<TReleaseWeatherOverride> releaseWeatherOverride(2208862);
    TiltedPhoques::ThisCall(releaseWeatherOverride, this);
}

void Sky::ResetWeather() noexcept
{
    TP_THIS_FUNCTION(TResetWeather, void, Sky);
    static VersionDbPtr<TResetWeather> resetWeather(2208860);
    TiltedPhoques::ThisCall(resetWeather, this);
}

TESWeather* Sky::GetWeather() const noexcept
{
    return pCurrentWeather;
}
