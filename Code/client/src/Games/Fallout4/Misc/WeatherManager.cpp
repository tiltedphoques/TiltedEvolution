#if TP_FALLOUT4

#include <Games/Fallout4/Misc/WeatherManager.h>

WeatherManager* WeatherManager::Get()
{
    POINTER_FALLOUT4(WeatherManager*, s_instance, 0x145AC64F0 - 0x140000000);

    return *s_instance.Get();
}

#endif
