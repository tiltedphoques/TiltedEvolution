#include <TiltedOnlinePCH.h>

#include <Misc/GameVM.h>

SkyrimVM* SkyrimVM::Get()
{
    POINTER_SKYRIMSE(SkyrimVM*, s_instance, 400475);

    return *s_instance.Get();
}
