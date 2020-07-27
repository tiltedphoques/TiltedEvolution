#if TP_SKYRIM

#include <Games/Skyrim/Misc/SkyrimVM.h>

SkyrimVM* SkyrimVM::Get()
{
    POINTER_SKYRIMSE(SkyrimVM*, s_instance, 0x141EC3B78 - 0x140000000);

    return *s_instance.Get();
}

#endif
