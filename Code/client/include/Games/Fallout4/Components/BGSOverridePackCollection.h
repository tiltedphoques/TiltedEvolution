#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct BGSOverridePackCollection : BaseFormComponent
{
    virtual ~BGSOverridePackCollection();

    void* packs[5];
};

#endif
