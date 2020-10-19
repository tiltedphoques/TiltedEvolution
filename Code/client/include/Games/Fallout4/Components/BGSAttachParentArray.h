#pragma once

#include <Components/BaseFormComponent.h>

struct BGSAttachParentArray : BaseFormComponent
{
    virtual ~BGSAttachParentArray();

    void* arr;
    uint32_t count;
    uint32_t pad;
};
