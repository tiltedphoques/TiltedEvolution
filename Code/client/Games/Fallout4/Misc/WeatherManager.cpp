#include <TiltedOnlinePCH.h>

#include <Misc/WeatherManager.h>

WeatherManager* WeatherManager::Get()
{
    POINTER_FALLOUT4(WeatherManager*, s_instance, 7492);

    return *s_instance.Get();
}
