#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct TESWorldSpace : TESForm 
{
    virtual ~TESWorldSpace();

    // aX and aY are coordinates, not positions
    TESObjectCELL* LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept;

    TESFullName fullName;
};
