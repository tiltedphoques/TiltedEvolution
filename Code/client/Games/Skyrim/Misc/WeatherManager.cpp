#include <TiltedOnlinePCH.h>

#include <Misc/WeatherManager.h>

WeatherManager* WeatherManager::Get()
{
    POINTER_SKYRIMSE(WeatherManager*, s_instance, 0x142FD3650 - 0x140000000);

    return *s_instance.Get();
}
