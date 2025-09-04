#include "ActionReplayCache.h"
#include <Game/Animation/AnimationEventLists.h>

void ActionReplayCache::AppendAll(const Vector<ActionEvent>& acActions) noexcept
{
    for (const auto& action : acActions)
    {
        if (AnimationEventLists::g_actionsIgnore.contains(action.EventName))
            continue;
        Actions.push_back(action);
    }

    if (Actions.size() > kReplayCacheMaxSize)
        Actions.erase(Actions.begin(), Actions.end() - kReplayCacheMaxSize);

    TruncateReplayChain();
}

void ActionReplayCache::TruncateReplayChain() noexcept
{
    bool startActionFound = false;
    int dropAllUpToIndex = -1;

    for (int i = Actions.size() - 1; i >= 0; --i)
    {
        const auto& action = Actions[i];

#if 0
        if (IsStartAction(action))
            startActionFound = true;
#endif
        // Terminate when an "exit" action is found. It is very likely that the actor has 
        // returned to its default/idle state, so we can start the chain from there
        if (IsExitAction(action))
        {
            dropAllUpToIndex = i;
            break;
        }

#if 0
        if (!startActionFound)
            continue;

        // Skip unimportant actions in search for the next non-start action
        if (ShouldSkipIntermediateAction(action))
            continue;
        // In one of the previous iterations a start action was found, and now we've found
        // a non-starting action, meaning we can cut the chain here
        if (!IsStartAction(action))
        {
            dropAllUpToIndex = i + 1;
            break;
        }
#endif
    }

    if (dropAllUpToIndex == -1)
        return;

    Actions.erase(Actions.begin(), Actions.begin() + dropAllUpToIndex);
}

bool ActionReplayCache::IsStartAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsStart.contains(acAction.EventName);
}

bool ActionReplayCache::IsExitAction(const ActionEvent& acAction) noexcept
{
    return AnimationEventLists::g_actionsExit.contains(acAction.EventName);
}

bool ActionReplayCache::ShouldSkipIntermediateAction(const ActionEvent& acAction) noexcept
{
    return false; // AnimationEventLists::g_actionsSkipIntermediate.contains(acAction.EventName);
}
