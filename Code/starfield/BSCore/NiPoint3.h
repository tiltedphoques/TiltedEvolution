#pragma once

struct NiPoint3
{
    float x;
    float y;
    float z;
};

static_assert(sizeof(NiPoint3) == 0xC);
