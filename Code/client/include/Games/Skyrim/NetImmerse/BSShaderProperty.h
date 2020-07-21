#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiShadeProperty.h>

struct BSShaderProperty : NiShadeProperty
{
    virtual ~BSShaderProperty();

    uint8_t pad30[0x48];

    void* material;
};

static_assert(offsetof(BSShaderProperty, material) == 0x78);

#endif
