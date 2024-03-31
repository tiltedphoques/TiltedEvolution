#pragma once

struct TESLeveledList : BaseFormComponent
{
    struct LEVELED_OBJECT
    {
        TESForm* pForm;
        uint16_t count;
        uint16_t level;
        uint32_t padC;
        void* pItemExtra;
    };

    LEVELED_OBJECT* pLeveledListA; // this array has count at offset -8
    uint8_t unk10[0x28 - 0x10];
};

static_assert(sizeof(TESLeveledList) == 0x28);
