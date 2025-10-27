#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/ActionEvent.h>

/**
 * @brief `LocalAnimationComponent` is typically added several frames after the Actor 
 * has been spawned, causing some of the earliest actions to be missed and not sent 
 * over the network. `EarlyAnimationBufferComponent` takes care of this.
 */
struct EarlyAnimationBufferComponent
{
    Vector<ActionEvent> Actions{};
};
