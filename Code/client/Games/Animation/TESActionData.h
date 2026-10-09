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
};

#if TP_PLATFORM_64
static_assert(offsetof(TESActionData, eventName) == 0x28);
#if defined(TP_FALLOUT4)
static_assert(offsetof(TESActionData, idleForm) == 0x40);
static_assert(offsetof(TESActionData, someFlag) == 0x58);
#else
static_assert(offsetof(TESActionData, idleForm) == 0x48);
#endif
#endif
