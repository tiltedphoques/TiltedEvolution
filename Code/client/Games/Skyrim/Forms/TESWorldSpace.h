#pragma once

#include <Forms/TESForm.h>

#include <Components/TESFullName.h>

struct TESWorldSpace : TESForm
{
    TESObjectCELL* LoadCell(int32_t aX, int32_t aY) noexcept;

    TESFullName fullName;
};
