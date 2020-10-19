#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct TESRace : TESForm
{
    virtual ~TESRace();

    TESFullName name;
};
