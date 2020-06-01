#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Common.h>

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

#endif
