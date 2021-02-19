#pragma once

#include <Components/BaseFormComponent.h>

struct BGSOverridePackCollection : BaseFormComponent
{
    virtual ~BGSOverridePackCollection();

    void* packs[5];
};

