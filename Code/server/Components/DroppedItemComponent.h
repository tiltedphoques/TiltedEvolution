#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/DroppedItemData.h>

/**
 * @brief An item lying in the world after a player dropped it.
 *
 * The entity also has a CellIdComponent so it can be sent to players in range.
 */
struct DroppedItemComponent
{
    // ServerId is left at 0, the entity itself is the id.
    DroppedItemData Data{};
    // Increases with every drop, the oldest items get cleaned up first when there are too many.
    uint64_t DropOrder{};
};
