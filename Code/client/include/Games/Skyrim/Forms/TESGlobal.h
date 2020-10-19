#pragma once

#include <Forms/TESForm.h>
#include <Misc/BSString.h>

struct TESGlobal : TESForm
{
    BSString unk14;
    uint8_t unk1C;
    uint8_t pad[3];
    union
    {
        uint32_t i;
        float f;
    };
};

static_assert(offsetof(TESGlobal, f) == 0x34);
