#include <stdafx.h>

#include <Misc/GameVM.h>

GameVM* GameVM::Get()
{
    POINTER_FALLOUT4(GameVM*, s_instance, 0x1458D3308 - 0x140000000);

    return *s_instance.Get();
}

