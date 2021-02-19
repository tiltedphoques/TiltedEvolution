#pragma once

#include <Components/BaseFormComponent.h>
#include <Misc/BSFixedString.h>

struct TESFullName : BaseFormComponent
{
    virtual ~TESFullName();

    BSFixedString value;
};
