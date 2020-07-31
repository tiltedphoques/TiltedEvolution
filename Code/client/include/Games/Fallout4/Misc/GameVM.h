#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Misc/BSScript.h>

struct GameVM
{
    virtual ~GameVM();

    static GameVM* Get();

    uint8_t pad8[0xB0 - 0x8];
    BSScript::IVirtualMachine* virtualMachine;
};

#endif
