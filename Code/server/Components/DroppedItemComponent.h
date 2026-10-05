#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/DroppedItemData.h>

struct Player;

/**
 * @brief An item a player dropped into the world. The server only registers it and relays its state;
 * the dropping client simulates the physics until the item settles.
 */
struct DroppedItemComponent
{
    Inventory::Entry Item{};
    DroppedItemTransform Transform{};
    bool IsAtRest{};
    // The client that simulates the item's physics; null once the item is at rest.
    Player* pSimulator{};
    // Increases with every drop; used to evict the oldest items first.
    uint64_t Sequence{};
};
