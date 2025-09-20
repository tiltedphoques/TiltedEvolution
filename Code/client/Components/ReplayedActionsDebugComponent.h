#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/ActionReplayChain.h>

/**
 * @brief Used for in-game animation debugging purposes. Makes it easier
 * to see which actions were replayed on a particular Actor.
 * 
 * @see ComponentView
 */
struct ReplayedActionsDebugComponent
{
    ActionReplayChain ActionsReceivedForReplay;
};
