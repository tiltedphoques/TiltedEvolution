#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESObject.h>

struct TESBoundObject : TESObject
{
    struct Bound
    {
        uint16_t x;
        uint16_t y;
        uint16_t z;
    };

    Bound upper;
    Bound lower;
};

#endif
