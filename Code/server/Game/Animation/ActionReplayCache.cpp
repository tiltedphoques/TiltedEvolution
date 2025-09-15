#include "ActionReplayCache.h"
#include <Game/Animation/AnimationEventLists.h>
#include <optional>

ActionReplayChain ActionReplayCache::GetReplayChain() const noexcept
{
    ActionReplayChain chain;
    chain.Actions = m_actions;
    chain.ResetAnimationGraph = m_isGraphResetNeeded;
    return chain;
}

void ActionReplayCache::AppendAll(const Vector<ActionEvent>& acActions) noexcept
{
    for (const auto& action : acActions)
    {
        if (ShouldIgnoreAction(action))
            continue;
        m_actions.push_back(action);
    }

    if (m_actions.size() > kReplayCacheMaxSize)
        m_actions.erase(m_actions.begin(), m_actions.end() - kReplayCacheMaxSize);

    RefineReplayCache();
}

void ActionReplayCache::RefineReplayCache() noexcept
{
	m_isGraphResetNeeded = false;

    int32_t dropAllUpToIndex = -1;

    for (int32_t i = m_actions.size() - 1; i >= 0; --i)
    {
        ActionEvent& action = m_actions[i];

        // Instant counterparts of actions are highly preferred for animation replay
        if (std::optional<String> instantAnimName = FindInstantCounterpartForAction(action.EventName))
        {
            action.EventName = *instantAnimName;
            action.TargetEventName = *instantAnimName;
            action.ActionId = 0;
            action.IdleId = 0;
        }

        if (IsExitAction(action))
        {
            // An "exit" action is found, meaning the actor has likely returned to its root/normal 
			// state, and the clients should reset the animation state before running replay
			m_isGraphResetNeeded = true;
            dropAllUpToIndex = i;
            break;
        }
    }

    if (dropAllUpToIndex == -1)
        return;

    m_actions.erase(m_actions.begin(), m_actions.begin() + dropAllUpToIndex);
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
    auto it = AnimationEventLists::g_actionsIdle.find(acAction);
    if (it != AnimationEventLists::g_actionsIdle.end())
    {
        auto& [_, instantAnimationName] = *it;
        if (!instantAnimationName.empty())
        {
            return {instantAnimationName};
        }
    }
    return std::nullopt;
}
