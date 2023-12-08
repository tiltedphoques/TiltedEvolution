#pragma once

#include "BaseFormComponent.h"

// TODO: reverse this
struct BSFixedString;

struct TESFullName : BaseFormComponent
{
    void sub_B();
    void sub_C();
    void sub_D();

    BSFixedString* strFullName; // TODO: this shouldn't be a ptr
};
