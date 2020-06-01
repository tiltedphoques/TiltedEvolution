#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>
#include <Games/Skyrim/Misc/BSFixedString.h>


struct TESFullName : BaseFormComponent
{
    virtual ~TESFullName();

    BSFixedString value;
};

#endif
