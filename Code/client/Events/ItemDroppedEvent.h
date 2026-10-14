#pragma once

#include <Structs/Inventory.h>

/**
 * @brief Dispatched when a locally owned actor dropped an item into the world.
 */
struct ItemDroppedEvent
{
    uint32_t ActorId{};
    // The reference the drop created.
    uint32_t RefId{};
    // Count is the (positive) size of the dropped stack.
    Inventory::Entry Item{};
};
