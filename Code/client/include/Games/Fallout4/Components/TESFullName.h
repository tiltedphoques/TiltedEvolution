#pragma once

#include <Components/BaseFormComponent.h>
#include <Misc/BSFixedString.h>

struct TESFullName : BaseFormComponent
{
    virtual ~TESFullName();

    virtual void sub_7();
    virtual void sub_8();

    BSFixedString value;
};
