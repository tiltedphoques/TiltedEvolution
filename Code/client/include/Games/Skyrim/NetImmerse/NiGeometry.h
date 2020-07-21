#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiPointer.h>
#include <Games/Skyrim/NetImmerse/NiAVObject.h>
#include <Games/Skyrim/NetImmerse/NiProperty.h>

struct NiGeometry : NiAVObject
{
    virtual ~NiGeometry();

    NiPointer<NiProperty> unkProperty1;
    NiPointer<NiProperty> unkProperty2;
    uintptr_t unkB0;
    NiPointer<NiProperty> effect;
    uintptr_t unkB8;
};

#endif
