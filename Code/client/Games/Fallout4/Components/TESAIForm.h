#pragma once

#include <Components/BaseFormComponent.h>

struct TESAIForm : BaseFormComponent
{
    uint8_t	unk8[0x28 - 0x8];
};

static_assert(sizeof(TESAIForm) == 0x28);
