#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Components/TESFullName.h>

struct BGSColorForm : TESForm
{
    TESFullName fullName;

    union
    {
        struct Color
        {
            uint8_t   red, green, blue, alpha;
        } color;
        uint32_t abgr;
    };
    uint32_t  unk20;
};

#endif
