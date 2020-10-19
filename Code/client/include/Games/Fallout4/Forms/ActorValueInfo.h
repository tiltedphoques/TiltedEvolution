#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct ActorValueInfo : TESForm
{
    virtual ~ActorValueInfo();

    TESFullName name;
    // TESDescription
};
