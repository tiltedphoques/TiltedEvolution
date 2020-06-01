#pragma once

#include <Games/Animation/ActionInput.h>
#include <Games/Animation/ActionOutput.h>

struct BGSActionData : ActionInput, ActionOutput
{
    enum SomeFlag : uint32_t
    {
        kTransitionNoAnimation = 1,
        kSkip = 2
    };

    BGSActionData(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget);
    virtual ~BGSActionData() {}

    virtual BGSActionData* Clone() { return nullptr; }
    virtual uintptr_t Perform() { return 0; }

    uint32_t someFlag;
    uint32_t pad5C;
};
