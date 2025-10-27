#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Game/Animation/ActionReplayCache.h>
#include <Structs/ActionEvent.h>

struct AnimationComponent
{
    Vector<ActionEvent> Actions;
    ActionEvent CurrentAction;
    ActionReplayCache ActionsReplayCache;
};
