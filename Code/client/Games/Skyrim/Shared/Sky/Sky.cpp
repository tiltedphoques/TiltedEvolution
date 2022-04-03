
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

void Sky::SetWeatherExternal(TESWeather* newWeather, bool overrideWeather, bool updateLastWeather)
{
    Sky_SetWeather(this, newWeather, overrideWeather, updateLastWeather);
}

// sub called from Sky::Update->UpdateWeather
uint32_t Hook_BSRandom_UnsignedInt(void* apGenerator, uint32_t aMax)
{
    if (SkyService::UseOnlineTick())
    {
        return World::Get().ctx<SkyService>().GetWeatherSeed();
    }

    return BSRandom_UnsignedInt(apGenerator, aMax);
}

static std::map<int, TiltedPhoques::String> g_NameMapping;

void DoSetID(TESForm* me, const char* name)
{
    g_NameMapping.insert(std::make_pair(me->formID, name));
}

const char* DoGetID(TESForm* me)
{
    auto it = g_NameMapping.find(me->formID);
    if (it != g_NameMapping.end())
        return (*it).second.c_str();

    return nullptr;
}

static TiltedPhoques::Initializer s_SkyInit([]() {
    TiltedPhoques::Put(0x141664D70, &DoGetID);
    TiltedPhoques::Put(0x141664D78, &DoSetID);

    VersionDbPtr<uint8_t> s_setWeather(26241);
    VersionDbPtr<uint8_t> s_getInstance(13878);
    Sky_SetWeather = static_cast<decltype(Sky_SetWeather)>(s_setWeather.GetPtr());
    Sky_GetInstance = static_cast<decltype(Sky_GetInstance)>(s_getInstance.GetPtr());

    // TODO(Force): examine if this is the only required location.
    TiltedPhoques::SwapCall(0x1402C226F, BSRandom_UnsignedInt, &Hook_BSRandom_UnsignedInt);
});
