#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/ActionEvent.h>

struct LocalAnimationComponent
{
    Vector<ActionEvent> Actions;
    ActionEvent LastProcessedAction;

    void Append(const ActionEvent& acEvent) noexcept { Actions.push_back(acEvent); }
};
