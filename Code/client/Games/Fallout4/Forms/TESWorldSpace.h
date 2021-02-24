#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct TESWorldSpace : TESForm 
{
    virtual ~TESWorldSpace();
    TESFullName fullName;
};
