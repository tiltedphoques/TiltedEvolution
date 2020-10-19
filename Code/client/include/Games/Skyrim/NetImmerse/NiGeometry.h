#pragma once

#include <NetImmerse/NiPointer.h>
#include <NetImmerse/NiAVObject.h>
#include <NetImmerse/NiProperty.h>

struct NiGeometry : NiAVObject
{
    virtual ~NiGeometry();

    NiPointer<NiProperty> unkProperty1;
    NiPointer<NiProperty> unkProperty2;
    uintptr_t unkB0;
    NiPointer<NiProperty> effect;
    uintptr_t unkB8;
};
