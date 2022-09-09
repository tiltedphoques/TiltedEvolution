#include <TiltedOnlinePCH.h>

#include <Misc/GameVM.h>

GameVM* GameVM::Get()
{
    POINTER_FALLOUT4(GameVM*, s_instance, 996228);

    return *s_instance.Get();
}

