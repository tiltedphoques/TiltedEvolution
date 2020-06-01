#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct TESAIForm : BaseFormComponent
{
    uint8_t	unk8[0x28 - 0x8];
};

static_assert(sizeof(TESAIForm) == 0x28);

#endif
