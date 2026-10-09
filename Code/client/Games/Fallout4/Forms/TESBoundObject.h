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
    uint32_t pad2C;
    uint8_t objectTemplate[0x20];
    uint8_t previewTransform[0x10];
    uint8_t soundTagComponent[0x8];
};

static_assert(sizeof(TESBoundObject) == 0x68);
