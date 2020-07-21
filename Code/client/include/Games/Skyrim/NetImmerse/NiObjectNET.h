#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiObject.h>

struct NiObjectNET : NiObject
{
    virtual ~NiObjectNET();

    uint8_t unk10[0x20];
};

#endif
