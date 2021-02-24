#include <TiltedOnlinePCH.h>

#include <Misc/GameVM.h>

SkyrimVM* SkyrimVM::Get()
{
    POINTER_SKYRIMSE(SkyrimVM*, s_instance, 0x141EC3B78 - 0x140000000);

    return *s_instance.Get();
}
