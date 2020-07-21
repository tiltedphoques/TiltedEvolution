#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>
#include <Games/Skyrim/Misc/BSFixedString.h>

struct TESTexture : BaseFormComponent
{
    virtual ~TESTexture() {};

    void Construct();

    BSFixedString name;
};

static_assert(offsetof(TESTexture, name) == 8);

#endif
