#pragma once

/**
 * @brief Dispatched when a locally owned actor picked up a temporary reference, which may be a synced dropped item.
 */
struct DroppedItemPickedUpEvent
{
    uint32_t ActorId{};
    uint32_t RefId{};
    uint32_t BaseFormId{};
};
