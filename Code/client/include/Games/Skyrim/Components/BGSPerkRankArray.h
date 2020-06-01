#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>

struct BGSPerkRankArray : BaseFormComponent
{
    void* perkRanks;
    uint32_t count;
};

#endif
