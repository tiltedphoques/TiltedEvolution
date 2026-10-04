#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;

struct TESContainer : BaseFormComponent
{
    struct Entry
    {
        uint32_t count;
        TESForm* form;
        void* data;
    };

    int64_t GetItemCount(TESForm* apItem) const noexcept;

    Entry** entries;
    uint32_t count;
};
