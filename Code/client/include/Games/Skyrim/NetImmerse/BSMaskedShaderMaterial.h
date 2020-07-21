#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiPointer.h>
#include <Games/Skyrim/NetImmerse/NiTexture.h>

struct BSMaskedShaderMaterial
{
    uint8_t pad0[0xA0];

    NiPointer<NiTexture> renderedTexture;
};

static_assert(offsetof(BSMaskedShaderMaterial, renderedTexture) == 0xA0);


#endif
