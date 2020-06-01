#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct TESForm;

struct BGSPerkRankArray : BaseFormComponent
{
    virtual ~BGSPerkRankArray();

    TESForm** perks; // 8
    uint32_t count; // 10
    uint32_t pad;
};

#endif
