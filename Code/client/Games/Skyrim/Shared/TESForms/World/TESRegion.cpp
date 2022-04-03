
#include "TESRegion.h"

TESWeather* (*TESRegion_SelectWeather)(TESRegion*){nullptr};

TESWeather* TESRegion::SelectWeather()
{
    return TESRegion_SelectWeather(this);
}

static TiltedPhoques::Initializer s_RegionInit([]() {
    const VersionDbPtr<uint8_t> s_selectWeather(16449);
    TESRegion_SelectWeather = static_cast<decltype(TESRegion_SelectWeather)>(s_selectWeather.GetPtr());
});
