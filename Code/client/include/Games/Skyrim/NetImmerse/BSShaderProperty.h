#pragma once

#include <NetImmerse/NiShadeProperty.h>

struct BSShaderProperty : NiShadeProperty
{
    virtual ~BSShaderProperty();

    uint8_t pad30[0x48];

    void* material;
};

static_assert(offsetof(BSShaderProperty, material) == 0x78);
