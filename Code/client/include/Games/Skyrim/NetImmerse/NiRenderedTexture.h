#pragma once

#include <NetImmerse/NiTexture.h>
#include <NetImmerse/Ni2DBuffer.h>

struct NiRenderedTexture : NiTexture
{
    virtual ~NiRenderedTexture();

    Ni2DBuffer* buffer;
};
