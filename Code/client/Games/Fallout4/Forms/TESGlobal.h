#pragma once

#include <Forms/TESForm.h>
#include <Misc/BSString.h>

struct TESGlobal : TESForm
{
    BSString unk14;
    union
    {
        uint32_t i;
        float f;
    };
};

static_assert(offsetof(TESGlobal, f) == 0x30);

static_assert(sizeof(TESGlobal) == 0x38);
