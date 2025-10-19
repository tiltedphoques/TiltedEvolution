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
        if (auto instantAnimName = FindInstantCounterpartForAction(action.EventName))
        {
            action.EventName = String{*instantAnimName};
            action.TargetEventName = String{*instantAnimName};
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
    if (AnimationEventLists::kExitSpecial.contains(acAction.EventName))
        return true;

    // Not found in special exit actions, let's check the suffix
    const String& originalName = acAction.EventName;
    String lowercaseName{acAction.EventName};
    std::transform(originalName.begin(), originalName.end(), lowercaseName.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return lowercaseName.starts_with("idle") &&
           (lowercaseName.ends_with("exit") || lowercaseName.ends_with("exitstart"));
}

bool ActionReplayCache::ShouldIgnoreAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::kIgnore.contains(acAction.EventName);
}

std::optional<std::string_view> ActionReplayCache::FindInstantCounterpartForAction(std::string_view aAction) noexcept
{
    auto it = AnimationEventLists::kIdleToInstant.find(aAction);
    if (it == AnimationEventLists::kIdleToInstant.end())
        return std::nullopt;

    auto& [_, instantAnimationName] = *it;
    if (!instantAnimationName.empty())
    {
        return {instantAnimationName};
    }
    return std::nullopt;
}
