#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

struct TESFullName : BaseFormComponent
{
    virtual ~TESFullName();

    virtual void sub_7();
    virtual void sub_8();

    BSFixedString value;
};

#endif
