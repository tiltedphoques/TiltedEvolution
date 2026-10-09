#pragma once

#include <Components/BaseFormComponent.h>

struct BGSOverridePackCollection : BaseFormComponent
{
    void* packages[6];
};

static_assert(sizeof(BGSOverridePackCollection) == 0x38);
