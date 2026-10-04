#pragma once

#include <Misc/BSFixedString.h>

struct TESIdleForm;

struct ActionOutput
{
    ActionOutput();
    ~ActionOutput() { Release(); }

    void Release();

    BSFixedString eventName;       // 28
    BSFixedString targetEventName; // 30
    int result;                    // 38
#if defined(TP_FALLOUT4)
    TESIdleForm* idleForm;         // 40 sequence
    TESIdleForm* targetIdleForm;   // 48 anim object idle
#else
    TESIdleForm* targetIdleForm;   // 40
    TESIdleForm* idleForm;         // 48
#endif
    uint32_t unk1;
};
