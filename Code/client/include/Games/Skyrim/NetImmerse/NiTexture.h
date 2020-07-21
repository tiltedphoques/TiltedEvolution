#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiObject.h>

struct NiTexture : NiObject
{
    virtual ~NiTexture();

    uint8_t unk10[0x38];
};

#endif
