#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <ActionEvent.h>
#include <Components/AnimationVariablesComponent.h>

struct LocalAnimationComponent
{
    Vector<ActionEvent> Actions;
    ActionEvent LastProcessedAction;

    [[nodiscard]] Outcome<ActionEvent, bool> GetLatestAction() const noexcept
    {
        if (Actions.empty())
            return false;

        return Actions[Actions.size() - 1];
    }

    void Append(const ActionEvent& acEvent) noexcept
    {
        Actions.push_back(acEvent);
    }
};
