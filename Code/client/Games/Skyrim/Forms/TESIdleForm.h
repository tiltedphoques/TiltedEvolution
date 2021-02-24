#pragma once

#include <Forms/TESForm.h>

struct TESIdleForm : TESForm
{
    uint8_t pad20[0xC];
    float value; // If this is > 0.0f it gets added to an array in the refr's loaded state
};
