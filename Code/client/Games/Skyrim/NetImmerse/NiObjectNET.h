#pragma once

#include <NetImmerse/NiObject.h>

struct NiObjectNET : NiObject
{
    virtual ~NiObjectNET();

    uint8_t unk10[0x20];
};
