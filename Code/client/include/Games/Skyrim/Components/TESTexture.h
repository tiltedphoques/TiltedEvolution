#pragma once

#include <Components/BaseFormComponent.h>
#include <Misc/BSFixedString.h>

struct TESTexture : BaseFormComponent
{
    virtual ~TESTexture() {};

    void Construct();

    BSFixedString name;
};

static_assert(offsetof(TESTexture, name) == 8);
