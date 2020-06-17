#include <Systems/AnimationSystem.h>

#include <Games/Animation/TESActionData.h>
#include <Games/Animation/ActorMediator.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/BGSAction.h>
#include <Games/Fallout4/Forms/BGSAction.h>

#include <Games/Fallout4/Misc/ProcessManager.h>
#include <Games/Fallout4/Misc/MiddleProcess.h>

#include <Games/Skyrim/Misc/ActorProcessManager.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>

#include <Components.h>
#include <World.h>
#include "ViewBuffer.hpp"

extern thread_local const char* g_animErrorCode;

void AnimationSystem::Update(World& aWorld, Actor* apActor, RemoteAnimationComponent& aAnimationComponent, const uint64_t aTick) noexcept
{
    auto& actions = aAnimationComponent.TimePoints;

    for(auto it = std::begin(actions); it != std::end(actions); )
    {
        const auto& first = *it;

        const auto actionId = aWorld.GetModSystem().GetGameId(first.action_id().mod(), first.action_id().base());
        const auto targetId = aWorld.GetModSystem().GetGameId(first.target_id().mod(), first.target_id().base());

        const auto pAction = RTTI_CAST(TESForm::GetById(actionId), TESForm, BGSAction);
        const auto pTarget = RTTI_CAST(TESForm::GetById(targetId), TESForm, TESObjectREFR);

        apActor->actorState.flags1 = first.state_1();
        apActor->actorState.flags2 = first.state_2();

        auto& variables = first.variables();

        /*apActor->SetAnimationFlags(first.flags());

        if(!variables.empty())
        {
            Buffer buffer((uint8_t*)variables.data(), variables.size());
            Buffer::Reader reader(&buffer);

            apActor->LoadAnimationVariables(reader);
        }*/

        // Play the animation
        TESActionData actionData(first.type() & 0x3, apActor, pAction, pTarget);
        actionData.eventName = BSFixedString(first.text().c_str());
        actionData.idleForm = RTTI_CAST(TESForm::GetById(first.idle_id()), TESForm, TESIdleForm);
        actionData.someFlag = ((first.type() & 0x4) != 0) ? 1 : 0;

        const auto result = ActorMediator::Get()->ForceAction(&actionData);

        actions.pop_front();
        it = std::begin(actions);
    }
}

void AnimationSystem::Setup(World& aWorld, const entt::entity aEntity) noexcept
{
    aWorld.emplace<RemoteAnimationComponent>(aEntity);
}

void AnimationSystem::AddAction(RemoteAnimationComponent& aAnimationComponent, const TiltedMessages::ActionData& acActionData) noexcept
{
    auto itor = std::begin(aAnimationComponent.TimePoints);
    const auto end = std::cend(aAnimationComponent.TimePoints);

    while (itor != end)
    {
        if (itor->tick() > acActionData.tick())
        {
            aAnimationComponent.TimePoints.insert(itor, acActionData);

            return;
        }

        ++itor;
    }

    aAnimationComponent.TimePoints.push_back(acActionData);
}

void AnimationSystem::Serialize(World& aWorld, TiltedMessages::ReferenceMovementSnapshot& aMovementSnapshot, LocalComponent& localComponent, LocalAnimationComponent& animationComponent, FormIdComponent& formIdComponent)
{
    const auto pForm = TESForm::GetById(formIdComponent.Id);
    const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
    if (!pActor)
        return;

    auto pEntry = aMovementSnapshot.add_entries();
    pEntry->set_server_id(localComponent.Id);

    TiltedMessages::Movement& movement = *pEntry->mutable_movement();;
    movement.mutable_position()->set_x(pActor->position.x);
    movement.mutable_position()->set_y(pActor->position.y);
    movement.mutable_position()->set_z(pActor->position.z);

    movement.mutable_rotation()->set_x(pActor->rotation.x);
    movement.mutable_rotation()->set_z(pActor->rotation.z);

    auto* pActorActions = pEntry->mutable_actions()->mutable_actions();
    for (auto& actionEvent : animationComponent.Actions)
    {
        const auto pAction = pActorActions->Add();

        // Try to serialize, if it fails remove it
        if(!Serialize(aWorld, actionEvent , *pAction))
            pActorActions->RemoveLast();
    }

    auto latestAction = animationComponent.GetLatestAction();

    if (latestAction)
        localComponent.CurrentAction = latestAction.MoveResult();

    animationComponent.Actions.clear();

    spdlog::info("Size {}", movement.ByteSizeLong());

   // if (!actorActions.actions().empty())
   //     pActions->operator[](localComponent.Id) = std::move(actorActions);
}

bool AnimationSystem::Serialize(World& aWorld, const ActionEvent& aActionEvent, TiltedMessages::ActionData& aActionData)
{
    uint32_t actionBaseId = 0;
    uint32_t actionModId = 0;
    if (!aWorld.GetModSystem().GetServerModId(aActionEvent.ActionId, actionModId, actionBaseId))
        return false;

    uint32_t targetBaseId = 0;
    uint32_t targetModId = 0;
    if (!aWorld.GetModSystem().GetServerModId(aActionEvent.TargetId, targetModId, targetBaseId))
        return false;

    // TODO: Send target id, have to resolve the server id first and then send it

    aActionData.mutable_action_id()->set_base(actionBaseId);
    aActionData.mutable_action_id()->set_mod(actionModId);
    aActionData.mutable_target_id()->set_base(targetBaseId);
    aActionData.mutable_target_id()->set_mod(targetModId);
    aActionData.set_tick(aActionEvent.Tick);
    aActionData.set_idle_id(aActionEvent.IdleId);
    aActionData.set_state_1(aActionEvent.State1);
    aActionData.set_state_2(aActionEvent.State2);
    aActionData.set_type(aActionEvent.Type);
    aActionData.set_text(aActionEvent.EventName.c_str());

    //aActionData.mutable_variables()->assign(aActionEvent.Variables.c_str(), aActionEvent.Variables.size());

    return true;
}
