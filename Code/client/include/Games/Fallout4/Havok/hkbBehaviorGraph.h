#pragma once

#include <Havok/hkbVariableValueSet.h>

struct hkbBehaviorGraph
{
    virtual ~hkbBehaviorGraph();

    uint8_t pad8[0x110 - 0x8];
    hkbVariableValueSet<uint32_t>* animationVariables; // 110
};
