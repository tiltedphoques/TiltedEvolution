
#include "TESRegion.h"
#include "TESWeather.h"

TESWeather* (*TESRegion_SelectWeather)(TESRegion*){nullptr};
void (*TESRegion_SetCurrentWeather)(TESRegion*, TESWeather*){nullptr};

TESWeather* TESRegion::SelectWeather()
{
    return TESRegion_SelectWeather(this);
}

void TESRegion::SetCurrentWeather(TESWeather* apWeather)
{
    TESRegion_SetCurrentWeather(this, apWeather);
}

void HOOK_TESRegion_SetCurrentWeather(TESRegion* apSelf, TESWeather* apWeather)
{
    const void* rsp = _ReturnAddress();

    spdlog::info("CW: {}, {} : {}", rsp, apSelf->GetFormEditorID(),
                 apWeather ? apWeather->GetFormEditorID() : "nullptr");

    TESRegion_SetCurrentWeather(apSelf, apWeather);
}

static TiltedPhoques::Initializer s_RegionInit([]() {
    const VersionDbPtr<uint8_t> s_selectWeather(16449);
    TESRegion_SelectWeather = static_cast<decltype(TESRegion_SelectWeather)>(s_selectWeather.GetPtr());

    const VersionDbPtr<uint8_t> s_setCurrentWeather(16448);
    TESRegion_SetCurrentWeather = static_cast<decltype(TESRegion_SetCurrentWeather)>(s_setCurrentWeather.GetPtr());

    // debug hook.
    TP_HOOK_IMMEDIATE(&TESRegion_SetCurrentWeather, &HOOK_TESRegion_SetCurrentWeather);
});
