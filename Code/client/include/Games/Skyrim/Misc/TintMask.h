#pragma once

#include <Components/TESTexture.h>

struct TintMask
{
    TintMask()
    {
        texture = nullptr;
        color = 0;
        alpha = 0.0f;
        type = 0;
    }

    ~TintMask()
    {
    }

    TESTexture* texture;
    uint32_t color;
    float alpha;
    uint32_t type;
};

