#include <Games/References.h>

#include <Games/Skyrim/Forms/BGSAction.h>
#include <Games/Skyrim/Forms/TESIdleForm.h>

#include <Games/Fallout4/Forms/BGSAction.h>
#include <Games/Fallout4/Forms/TESIdleForm.h>

#include <Events/ActionEvent.h>

#include <Games/Animation/ActorMediator.h>
#include <Games/Animation/TESActionData.h>

#include <Games/Skyrim/Misc/BSFixedString.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

#include <World.h>

TP_THIS_FUNCTION(TPerformAction, uint8_t, ActorMediator, TESActionData* apAction);
static TPerformAction* RealPerformAction;

thread_local bool g_forceAnimation = false;

uint8_t TP_MAKE_THISCALL(HookPerformAction, ActorMediator, TESActionData* apAction)
{
    auto pActor = apAction->actor;
    const auto pExtension = pActor->GetExtension();

    if (!pExtension->IsRemote())
    {
        ActionEvent action;
        action.State1 = pActor->actorState.flags1;
        action.State2 = pActor->actorState.flags2;

        action.Type = apAction->unkInput;

        const auto res = ThisCall(RealPerformAction, apThis, apAction);

        if (res != 0)
        {
            action.Tick = World::Get().GetTick();
            action.ActorId = pActor->formID;
            action.ActionId = apAction->action->formID;
            action.TargetId = apAction->target ? apAction->target->formID : 0;
            action.IdleId = apAction->idleForm ? apAction->idleForm->formID : 0;
            action.Variables = pActor->GetAnimationVariables();

            World::Get().GetRunner().Trigger(action);
        }

        return res;
    }

#if TP_FALLOUT4
    // Let the ActionInstantInitializeGraphToBaseState event go through
    if (apAction->action->formID == 0x5704c)
        return ThisCall(RealPerformAction, apThis, apAction);
#endif

    return 0;
}

ActorMediator* ActorMediator::Get() noexcept
{
    POINTER_FALLOUT4(ActorMediator*, s_actorMediator, 0x145AA4710 - 0x140000000);
    POINTER_SKYRIMSE(ActorMediator*, s_actorMediator, 0x142F271C0 - 0x140000000);

    return *(s_actorMediator.Get());
}

bool ActorMediator::PerformAction(TESActionData* apAction, float aValue) noexcept
{
    const auto res = ThisCall(RealPerformAction, this, apAction);

    return res != 0;
}

ActionInput::ActionInput(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget)
{
    // skip vtable as we never use this directly
    actor = apActor;
    target = apTarget;
    action = apAction;
    unkInput = aParam1;
}

void ActionInput::Release()
{
    actor.Release();
    target.Release();
}

ActionOutput::ActionOutput()
    : eventName("")
    , unkName("")
{
    // skip vtable as we never use this directly

    result = 0;
    unkPointer = nullptr;
    idleForm = nullptr;
    unk1 = 0;
}

void ActionOutput::Release()
{
    eventName.Release();
    unkName.Release();
}

BGSActionData::BGSActionData(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget)
    : ActionInput(aParam1, apActor, apAction, apTarget)
{
    // skip vtable as we never use this directly
    someFlag = 0;
}

TESActionData::TESActionData(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget)
    : BGSActionData(aParam1, apActor, apAction, apTarget)
{
    POINTER_FALLOUT4(void*, s_vtbl, 0x142C4A2A8 - 0x140000000);
    POINTER_SKYRIMSE(void*, s_vtbl, 0x141548198 - 0x140000000);

    someFlag = false;

    *reinterpret_cast<void**>(this) = s_vtbl.Get();
}

TESActionData::~TESActionData()
{
    ActionOutput::Release();
    ActionInput::Release();
}

static TiltedPhoques::Initializer s_animationHook([]()
{
    POINTER_FALLOUT4(TPerformAction, performAction, 0x140E20FB0 - 0x140000000);
    POINTER_SKYRIMSE(TPerformAction, performAction, 0x14063AF10 - 0x140000000);

    RealPerformAction = performAction.Get();

    TP_HOOK(&RealPerformAction, HookPerformAction);
});

