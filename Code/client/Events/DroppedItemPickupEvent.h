#pragma once

/**
 * @brief Dispatched when a local actor picks up a temporary (e.g. dropped) reference.
 */
struct DroppedItemPickupEvent
{
    uint32_t RefId{};
    uint32_t BaseId{};
};
