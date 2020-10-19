#include <Misc/WeatherManager.h>

WeatherManager* WeatherManager::Get()
{
    POINTER_SKYRIMSE(WeatherManager*, s_instance, 0x142F38978 - 0x140000000);

    return *s_instance.Get();
}
