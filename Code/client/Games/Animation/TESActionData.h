#pragma once

#include <Games/Animation/BGSActionData.h>

struct TESObjectREFR;
struct TESIdleForm;
struct Actor;
struct BGSAction;

struct TESActionData final : BGSActionData
{
    TESActionData(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget); // pass 2 for aParam1
    ~TESActionData();

    bool ComputeResult();
};

#if TP_PLATFORM_64
static_assert(offsetof(TESActionData, eventName) == 0x28);
static_assert(offsetof(TESActionData, idleForm) == 0x48);
#endif
