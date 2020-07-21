#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiTexture.h>
#include <Games/Skyrim/NetImmerse/Ni2DBuffer.h>

struct NiRenderedTexture : NiTexture
{
    virtual ~NiRenderedTexture();

    Ni2DBuffer* buffer;
};

#endif
