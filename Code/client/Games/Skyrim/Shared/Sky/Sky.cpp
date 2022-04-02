
#include "Shared/Sky/Sky.h"
#include "Shared/Sky/Moon.h"

#include <Services/SkyService.h>
#include <World.h>

Sky* Sky::GetInstance()
{
    POINTER_SKYRIMSE(Sky*, s_instance, 13878);
    return *s_instance.Get();
}

static void (*Sky_SetWeather)(Sky*, TESWeather*, bool, bool){nullptr};

void Sky::SetWeather(TESWeather* newWeather, bool overrideWeather, bool updateLastWeather)
{
    Sky_SetWeather(this, newWeather, overrideWeather, updateLastWeather);
}

uint32_t (*BSRandom_UnsignedInt)(void*, uint32_t){nullptr};

uint32_t Hook_BSRandom_UnsignedInt(void* apGenerator, uint32_t aMax)
{
    if (SkyService::UseOnlineTick())
    {
        return World::Get().ctx<SkyService>().GetWeatherSeed();
    }

    return BSRandom_UnsignedInt(apGenerator, aMax);
}

static TiltedPhoques::Initializer s_SkyInit([]() {
    POINTER_SKYRIMSE(decltype(*Sky_SetWeather), s_setWeather, 26241);
    Sky_SetWeather = s_setWeather.Get();

    // TODO(Force): examine if this is the only required location.
    TiltedPhoques::SwapCall(0x1402C226F, BSRandom_UnsignedInt, &Hook_BSRandom_UnsignedInt);
});
