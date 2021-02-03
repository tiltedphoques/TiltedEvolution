#pragma once

#include <Havok/hkbVariableValueSet.h>

struct hkEventContext;
struct hkEventType;
struct hkbGenerator;
struct hkbSymbolIdMap;
struct hkbStateMachine;

struct SomeData
{
    uint8_t pad0[0x58];
    hkbGenerator* generator; // 58
    struct hkbBehaviorGraph* behaviorGraph; // 60
    uint8_t pad68[0x84 - 0x68];
    uint8_t byte84; // 84
    uint8_t byte85; // 85
    uint8_t pad86[0x90 - 0x86];
};

static_assert(offsetof(SomeData, generator) == 0x58);
static_assert(offsetof(SomeData, behaviorGraph) == 0x60);
static_assert(offsetof(SomeData, byte84) == 0x84);

struct hkbBehaviorGraph
{
    virtual ~hkbBehaviorGraph();

    virtual void sub_01();
    virtual void sub_02();
    virtual void sub_03();
    virtual void sub_04();
    virtual void sub_05();
    virtual void SendEvent(hkEventContext& aContext, hkEventType& aType);

    // Reverse engineering do not use
    void ReSendEvent(hkEventContext& aContext, hkEventType& aType);
    void ReHandleEvent(hkEventContext& aContext, hkEventType& aType);

    struct Struct98
    {
        SomeData* data;
        int count;
    };

    uint8_t pad8[0x80 - 0x8];
    hkbStateMachine* stateMachine;
    uint8_t pad88[0x98 - 0x88];
    Struct98* struct98;          // 98
    uint8_t padA0[0xB8 - 0xA0];
    hkbSymbolIdMap* symbolIdMap; // B8
    uint8_t padC0[0xD8 - 0xC0];
    hkbVariableValueSet<uint32_t>* animationVariables; // D8
    uint8_t padE0[0x12C - 0xE0];
    uint8_t byte12C;
    uint8_t byte12D;
    uint8_t byte12E;
};

static_assert(offsetof(hkbBehaviorGraph, struct98) == 0x98);
static_assert(offsetof(hkbBehaviorGraph, symbolIdMap) == 0xB8);
static_assert(offsetof(hkbBehaviorGraph, animationVariables) == 0xD8);
static_assert(offsetof(hkbBehaviorGraph, byte12C) == 0x12C);

