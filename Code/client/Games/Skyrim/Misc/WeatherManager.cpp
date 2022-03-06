#include <TiltedOnlinePCH.h>

#include <Misc/WeatherManager.h>

WeatherManager* WeatherManager::Get()
{
    POINTER_SKYRIMSE(WeatherManager*, s_instance, 403759);

    return *s_instance.Get();
}
