#pragma once

struct hkbVariableInfo
{
    enum Type : int8_t
    {
        kBool = 0,
        kInt8 = 1,
        kInt16 = 2,
        kInt32 = 3,
        kReal = 4,
        // The value set word of the following types is an index into another array, not the value itself
        kPointer = 5,
        kVector3 = 6,
        kVector4 = 7,
        kQuaternion = 8,
    };

    int16_t role;
    int16_t roleFlags;
    Type type;
};

static_assert(sizeof(hkbVariableInfo) == 0x6);
static_assert(offsetof(hkbVariableInfo, type) == 0x4);

struct hkbBehaviorGraphData
{
    virtual ~hkbBehaviorGraphData();

    uint8_t pad8[0x20 - 0x8];
    hkbVariableInfo* variableInfos; // 20
    int32_t variableInfoCount;      // 28
};

static_assert(offsetof(hkbBehaviorGraphData, variableInfos) == 0x20);
static_assert(offsetof(hkbBehaviorGraphData, variableInfoCount) == 0x28);
