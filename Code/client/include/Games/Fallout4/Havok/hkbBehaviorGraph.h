#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Havok/hkbVariableValueSet.h>

struct hkbBehaviorGraph
{
    virtual ~hkbBehaviorGraph();

    uint8_t pad8[0x110 - 0x8];
    hkbVariableValueSet<uint32_t>* animationVariables; // 110
};

#endif
