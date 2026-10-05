#pragma once

#include <Havok/hkbVariableValueSet.h>

struct hkbStateMachine;

// Fallout 4 hkbVariableInfo: role (4 bytes) then the variable type.
struct hkbVariableInfo
{
    enum Type : int8_t
    {
        kBool = 0,
        kInt8 = 1,
        kInt16 = 2,
        kInt32 = 3,
        kReal = 4,
        kPointer = 5,
        kVector3 = 6,
        kVector4 = 7,
        kQuaternion = 8,
    };

    int16_t role;
    int16_t roleFlags;
    Type type;
    uint8_t pad5;
};
static_assert(sizeof(hkbVariableInfo) == 0x6);

// Fallout 4 hkbBehaviorGraphData: 0x70.
struct hkbBehaviorGraphData
{
    virtual ~hkbBehaviorGraphData();

    uint8_t pad8[0x20 - 0x8];
    hkbVariableInfo* variableInfos; // 20
    int32_t variableInfoCount;      // 28
};
static_assert(offsetof(hkbBehaviorGraphData, variableInfos) == 0x20);

// Fallout 4 hkbBehaviorGraph: 0x1B0.
struct hkbBehaviorGraph
{
    virtual ~hkbBehaviorGraph();

    uint8_t pad8[0xC0 - 0x8];
    hkbStateMachine* stateMachine;  // C0 m_rootGenerator
    hkbBehaviorGraphData* data;     // C8
    uint8_t padD0[0x110 - 0xD0];
    hkbVariableValueSet<uint32_t>* animationVariables; // 110 m_variableValueSet
    uint8_t pad118[0x1B0 - 0x118];
};

static_assert(offsetof(hkbBehaviorGraph, stateMachine) == 0xC0);
static_assert(offsetof(hkbBehaviorGraph, data) == 0xC8);
static_assert(offsetof(hkbBehaviorGraph, animationVariables) == 0x110);
static_assert(sizeof(hkbBehaviorGraph) == 0x1B0);
