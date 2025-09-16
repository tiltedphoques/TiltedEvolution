#include "ActionReplayCache.h"
#include <Game/Animation/AnimationEventLists.h>
#include <optional>

ActionReplayChain ActionReplayCache::FormRefinedReplayChain() noexcept
{
    const bool shouldResetAnimGraph = RefineReplayCache();

    ActionReplayChain chain;
    chain.ResetAnimationGraph = shouldResetAnimGraph;
    chain.Actions = (shouldResetAnimGraph && !m_actions.empty())
        ? Vector<ActionEvent>{m_actions.begin() + 1, m_actions.end()} // Drop the exit action
        : m_actions;

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
}

bool ActionReplayCache::RefineReplayCache() noexcept
{
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
            dropAllUpToIndex = i;
            break;
        }
    }

    if (dropAllUpToIndex == -1)
        return false;

    m_actions.erase(m_actions.begin(), m_actions.begin() + dropAllUpToIndex);
    return true;
}

bool ActionReplayCache::IsExitAction(const ActionEvent& acAction) noexcept
{
    if (AnimationEventLists::g_actionsExitSpecial.contains(acAction.EventName))
        return true;

    // Otherwise check string suffix
    const String& originalName = acAction.EventName;
    String lowercaseName{acAction.EventName};
    std::transform(originalName.begin(), originalName.end(), lowercaseName.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return lowercaseName.starts_with("idle") &&
           (lowercaseName.ends_with("exit") || lowercaseName.ends_with("exitstart"));
}

bool ActionReplayCache::ShouldIgnoreAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsIgnore.contains(acAction.EventName);
}

std::optional<String> ActionReplayCache::FindInstantCounterpartForAction(const String& acAction) noexcept
{
    auto it = AnimationEventLists::g_actionsIdleToInstant.find(acAction);
    if (it != AnimationEventLists::g_actionsIdleToInstant.end())
    {
        auto& [_, instantAnimationName] = *it;
        if (!instantAnimationName.empty())
        {
            return {instantAnimationName};
        }
    }
    return std::nullopt;
}
