#include "ActionReplayCache.h"
#include <Game/Animation/AnimationEventLists.h>

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

    TruncateReplayChain();
}

void ActionReplayCache::TruncateReplayChain() noexcept
{
    int dropAllUpToIndex = -1;

    for (int i = Actions.size() - 1; i >= 0; --i)
    {
        const auto& action = Actions[i];

        // Terminate when an "exit" action is found. It is very likely that the actor has
        // returned to its default/idle state, so we can start the chain from there
        if (IsExitAction(action))
        {
            dropAllUpToIndex = i;
            break;
        }
    }

    if (dropAllUpToIndex == -1)
        return;

    Actions.erase(Actions.begin(), Actions.begin() + dropAllUpToIndex);
}

// Currently unused
bool ActionReplayCache::IsStartAction(const ActionEvent& acAction) noexcept
{
    return false; // AnimationEventLists::g_actionsStart.contains(acAction.EventName);
}

bool ActionReplayCache::IsExitAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsExit.contains(acAction.EventName);
}

bool ActionReplayCache::ShouldIgnoreAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsIgnore.contains(acAction.EventName);
}
