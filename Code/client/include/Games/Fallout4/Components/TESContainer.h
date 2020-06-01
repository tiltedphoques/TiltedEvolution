#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct TESForm;

struct TESContainer : BaseFormComponent
{
    struct Entry
    {
        uint32_t count;
        TESForm* form;
        void* data;
    };

    Entry** entries;
    uint32_t count;
};

static_assert(sizeof(TESContainer) == 0x18);

#endif
