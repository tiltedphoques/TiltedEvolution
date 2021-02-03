#pragma once


#include <Forms/TESObject.h>

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

static_assert(sizeof(TESBoundObject) == 0x30);
