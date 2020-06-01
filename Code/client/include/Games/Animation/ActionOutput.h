#pragma once

#include <Games/Fallout4/Misc/BSFixedString.h>
#include <Games/Skyrim/Misc/BSFixedString.h>

struct TESIdleForm;

struct ActionOutput
{
    ActionOutput();
    ~ActionOutput() { Release(); }

    void Release();

    BSFixedString eventName;    // 28
    BSFixedString unkName;      // 30
    int result;                 // 38
    void* unkPointer;           // 40
    TESIdleForm* idleForm;      // 48
    uint32_t unk1;
};


