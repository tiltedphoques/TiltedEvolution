#pragma once

#include <Forms/TESForm.h>
#include <Misc/BSFixedString.h>

struct TESGlobal : TESForm
{
    BSFixedString name;
    uint8_t unk1C;
    uint8_t pad[3 + 4];
    union
    {
        uint32_t i;
        float f;
    };
};

static_assert(offsetof(TESGlobal, i) == 0x30);
