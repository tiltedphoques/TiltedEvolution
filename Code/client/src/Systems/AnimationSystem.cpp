#include <Systems/AnimationSystem.h>

#include <Games/Animation/TESActionData.h>
#include <Games/Animation/ActorMediator.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/BGSAction.h>
#include <Games/Fallout4/Forms/BGSAction.h>

#include <Components.h>
#include <World.h>

extern thread_local const char* g_animErrorCode;

void AnimationSystem::Update(World& aWorld, Actor* apActor, RemoteAnimationComponent& aAnimationComponent, const uint64_t aTick) noexcept
{
    auto& actions = aAnimationComponent.TimePoints;
    if (actions.empty())
        return;

    const auto& first = *(actions.begin());
    if(first.tick() <= aTick)
    {
        const auto actionId = aWorld.GetModSystem().GetGameId(first.action_id().mod(), first.action_id().base());
        const auto targetId = aWorld.GetModSystem().GetGameId(first.target_id().mod(), first.target_id().base());

        const auto pAction = RTTI_CAST(TESForm::GetById(actionId), TESForm, BGSAction);
        const auto pTarget = RTTI_CAST(TESForm::GetById(targetId), TESForm, TESObjectREFR);

        apActor->actorState.flags1 = first.state_1();
        apActor->actorState.flags2 = first.state_2();

        auto& variables = first.variables();

        apActor->SetAnimationVariables(Vector<uint32_t>(std::begin(variables), std::end(variables)));

        // Play the animation
        TESActionData actionData(first.type(), apActor, pAction, pTarget);
        actionData.idleForm = RTTI_CAST(TESForm::GetById(first.idle_id()), TESForm, TESIdleForm);
        const auto result = ActorMediator::Get()->PerformAction(&actionData, first.value());
        if(result == false)
        {
            spdlog::info("Animation failed: {} {} {}", actionData.action->keyword.AsAscii(), first.value(), g_animErrorCode);
        }

        actions.pop_front();
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
