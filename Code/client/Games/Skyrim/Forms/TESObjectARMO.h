#pragma once

#include <Forms/TESForm.h>

struct TESObjectARMO : TESForm
{
    uint8_t unk20[0x1B8 - sizeof(TESForm)];
    uint32_t slotType;
};
