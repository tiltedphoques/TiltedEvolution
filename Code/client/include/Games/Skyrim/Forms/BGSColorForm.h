#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

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
