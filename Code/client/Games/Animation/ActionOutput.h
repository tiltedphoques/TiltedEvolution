#pragma once

#include <Misc/BSFixedString.h>

struct TESIdleForm;

struct ActionOutput
{
    ActionOutput();
    ~ActionOutput() { Release(); }

    void Release();

    BSFixedString eventName;            // 28
    BSFixedString targetEventName;      // 30
    int result;                         // 38
    TESIdleForm* targetIdleForm;        // 40
    TESIdleForm* idleForm;              // 48
    uint32_t unk1;
};


