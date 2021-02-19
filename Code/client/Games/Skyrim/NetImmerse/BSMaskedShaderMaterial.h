#pragma once

#include <NetImmerse/NiPointer.h>
#include <NetImmerse/NiTexture.h>

struct BSMaskedShaderMaterial
{
    uint8_t pad0[0xA0];

    NiPointer<NiTexture> renderedTexture;
};

static_assert(offsetof(BSMaskedShaderMaterial, renderedTexture) == 0xA0);
