#pragma once

#include <Havok/hkbVariableValueSet.h>

struct hkbStateMachine;

struct hkbBehaviorGraph
{
    virtual ~hkbBehaviorGraph();

    uint8_t pad8[0xC0 - 0x8];
    hkbStateMachine* stateMachine;
    uint8_t padC8[0x110 - 0xC8];
    hkbVariableValueSet* animationVariables; // 110
};
