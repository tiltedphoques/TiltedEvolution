#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;
struct TESSpellList : BaseFormComponent
{
    struct Lists
    {
        void** spells;
        TESForm** unk4;
        void** shouts;
        uint32_t spellCount;
        uint32_t unk4Count;
        uint32_t shoultCount;
    };

    Lists* lists;

    void Initialize();
};

static_assert(sizeof(TESSpellList) == 0x10);
