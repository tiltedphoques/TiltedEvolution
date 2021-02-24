#pragma once

#include <Misc/BSScript.h>

struct SkyrimVM
{
    virtual ~SkyrimVM();

    static SkyrimVM* Get();

    uint8_t pad8[0x200 - 0x8];
    BSScript::IVirtualMachine* virtualMachine;
};

using GameVM = SkyrimVM;
