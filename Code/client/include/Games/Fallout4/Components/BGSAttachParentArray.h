#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct BGSAttachParentArray : BaseFormComponent
{
    virtual ~BGSAttachParentArray();

    void* arr;
    uint32_t count;
    uint32_t pad;
};

#endif
