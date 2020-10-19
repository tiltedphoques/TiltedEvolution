#pragma once

#include <Components/BaseFormComponent.h>

struct TESAIForm : BaseFormComponent
{
    struct IntrusiveEntry
    {
        uintptr_t unk;
        IntrusiveEntry* pNext;
    };

    uint32_t flags;
    uint8_t unkC[0xC];
    IntrusiveEntry head;
};
