#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Forms/TESForm.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

struct TESGlobal : TESForm
{
    BSFixedString unk14;
    uint8_t unk1C;
    uint8_t pad[3 + 4];
    union
    {
        uint32_t i;
        float f;
    };
};

static_assert(offsetof(TESGlobal, i) == 0x30);

#endif
