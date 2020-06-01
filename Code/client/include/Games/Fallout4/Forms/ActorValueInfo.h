#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Forms/TESForm.h>
#include <Games/Fallout4/Components/TESFullName.h>

struct ActorValueInfo : TESForm
{
    virtual ~ActorValueInfo();

    TESFullName name;
    // TESDescription
};

#endif
