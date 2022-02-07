#include "AIProcess.h"

#include <Actor.h>
#include <Games/ActorExtension.h>

void AIProcess::ClearPerks() noexcept
{
    TP_THIS_FUNCTION(TClearPerks, void, AIProcess);
    POINTER_SKYRIMSE(TClearPerks, clearPerks, 0x1406A95D0 - 0x140000000);
    ThisCall(clearPerks, this);
}

struct DialogueItem;

TP_THIS_FUNCTION(TProcessResponse, uint64_t, AIProcess, DialogueItem* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor);
static TProcessResponse* RealProcessResponse = nullptr;

uint64_t TP_MAKE_THISCALL(HookProcessResponse, AIProcess, DialogueItem* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor)
{
    if (apTalkingActor)
    {
        if (apTalkingActor->GetExtension()->IsRemotePlayer())
            return 0;
    }

    return ThisCall(RealProcessResponse, apThis, apVoice, apTalkingActor, apTalkedToActor);
}

TP_THIS_FUNCTION(TApplyPerksFromBase, void, AIProcess, Actor* apOwner);
static TApplyPerksFromBase* RealApplyPerksFromBase = nullptr;

void TP_MAKE_THISCALL(HookApplyPerksFromBase, AIProcess, Actor* apOwner)
{
    extern uint32_t s_nextPerkFormId;

    // mimics the in-game code
    if (apThis->middleProcess)
    {
        if (TESForm* pBase = apOwner->baseForm)
            s_nextPerkFormId = pBase->formID;
    }

    return;
}

static TiltedPhoques::Initializer s_aiProcessHooks([]() 
{
    POINTER_SKYRIMSE(TProcessResponse, s_processResponse, 0x14068BC50 - 0x140000000);
    POINTER_SKYRIMSE(TApplyPerksFromBase, s_applyPerksFromBase, 0x1406A9580 - 0x140000000);

    RealProcessResponse = s_processResponse.Get();
    RealApplyPerksFromBase = s_applyPerksFromBase.Get();

    TP_HOOK(&RealProcessResponse, HookProcessResponse);
    TP_HOOK(&RealApplyPerksFromBase, HookApplyPerksFromBase);
});
