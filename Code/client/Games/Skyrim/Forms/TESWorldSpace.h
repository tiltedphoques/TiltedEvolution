#pragma once

#include <Forms/TESForm.h>

#include <Components/TESFullName.h>

struct TESObjectCELL;

struct TESWorldSpace : TESForm
{
    TESObjectCELL* LoadCell(int32_t aX, int32_t aY) noexcept;

    TESFullName fullName;
    uint8_t pad30[0x28];
};
