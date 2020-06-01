#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>

struct TESAIForm : BaseFormComponent
{
    struct IntrusiveEntry
    {
        uintptr_t unk;
        IntrusiveEntry* pNext;
    };

    uint32_t flags;
#ifdef _WIN64
    uint8_t unkC[0xC];
#else
    uint16_t unk08[4];
#endif
    IntrusiveEntry head;
};

#endif
