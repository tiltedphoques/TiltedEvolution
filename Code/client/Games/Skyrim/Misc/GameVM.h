#pragma once

#include <Misc/BSScript.h>

struct SkyrimVM
{
    virtual ~SkyrimVM();

    static SkyrimVM* Get();

#ifdef TP_SKYRIM_1_6_1170
    uint8_t pad8[0x200 - 0x8];
    BSScript::IVirtualMachine* virtualMachine;
    uint8_t pad208[0x680 - 0x208];
#else
    uint8_t pad8[0x210 - 0x8];
    BSScript::IVirtualMachine* virtualMachine;
    uint8_t pad218[0x690 - 0x218];
#endif
    uint8_t inactive;
};

#ifdef TP_SKYRIM_1_6_1170
static_assert(offsetof(SkyrimVM, virtualMachine) == 0x200);
static_assert(offsetof(SkyrimVM, inactive) == 0x680);
#else
static_assert(offsetof(SkyrimVM, virtualMachine) == 0x210);
static_assert(offsetof(SkyrimVM, inactive) == 0x690);
#endif

using GameVM = SkyrimVM;
