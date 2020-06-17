#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct AnimationComponent
{
    Vector<TiltedMessages::ActionData> Actions;
    TiltedMessages::ActionData CurrentAction;
    TiltedMessages::ActionData CurrentVariables;
};
