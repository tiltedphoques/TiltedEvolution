#pragma once

#include <Misc/BSScript.h>

struct GameVM
{
    static GameVM* Get();

    uint8_t pad0[0xB0];
    BSScript::IVirtualMachine* virtualMachine;
    uint8_t padB8[0x610 - 0xB8];
    bool frozen;
};

static_assert(offsetof(GameVM, virtualMachine) == 0xB0);
static_assert(offsetof(GameVM, frozen) == 0x610);
