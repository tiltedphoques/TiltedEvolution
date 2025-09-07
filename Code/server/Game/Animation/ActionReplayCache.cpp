#include "ActionReplayCache.h"
#include <Game/Animation/AnimationEventLists.h>
#include <optional>

void ActionReplayCache::AppendAll(const Vector<ActionEvent>& acActions) noexcept
{
    for (const auto& action : acActions)
    {
        if (ShouldIgnoreAction(action))
            continue;
        Actions.push_back(action);
    }

    if (Actions.size() > kReplayCacheMaxSize)
        Actions.erase(Actions.begin(), Actions.end() - kReplayCacheMaxSize);

    RefineReplayChain();
}

void ActionReplayCache::RefineReplayChain() noexcept
{
    int dropAllUpToIndex = -1;

    for (int i = Actions.size() - 1; i >= 0; --i)
    {
        ActionEvent& action = Actions[i];

        // Instant counterparts of actions are highly preferred for animation replay
        if (std::optional<String> instantAnimName = FindInstantCounterpartForAction(action.EventName))
        {
            action.EventName = *instantAnimName;
            action.TargetEventName = *instantAnimName;
            // TODO: Reverse engineer PerformComplexAction (client code), because I'm not sure if these can be set to 0
            action.ActionId = 0;
            action.IdleId = 0;
        }

        if (IsExitAction(action))
        {
            // Replace this exit action with "IdleForceDefaultState" because
            // the actor has likely returned to its root/normal state
            action.EventName = AnimationEventLists::kIdleForceDefaultState;
            action.TargetEventName = AnimationEventLists::kIdleForceDefaultState;
            action.ActionId = 0;
            action.IdleId = 0;

            // Break when an "exit" action is found and start the chain from there
            dropAllUpToIndex = i;
            break;
        }
    }

    if (dropAllUpToIndex == -1)
        return;

    Actions.erase(Actions.begin(), Actions.begin() + dropAllUpToIndex);
}

bool ActionReplayCache::IsExitAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsExit.contains(acAction.EventName);
}

bool ActionReplayCache::ShouldIgnoreAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsIgnore.contains(acAction.EventName);
}

std::optional<String> ActionReplayCache::FindInstantCounterpartForAction(const String& acAction) noexcept
{
    auto it = AnimationEventLists::g_actionsStart.find(acAction);
    if (it != AnimationEventLists::g_actionsStart.end())
    {
        auto& [_, instantAnimationName] = *it;
        if (!instantAnimationName.empty())
        {
            return {instantAnimationName};
        }
    }
    return std::nullopt;
}
