#pragma once

#include <Structs/Inventory.h>

/**
 * @brief Dispatched when the local player drops an item into the world.
 */
struct DropItemEvent
{
    DropItemEvent(const uint32_t aFormId, Inventory::Entry aItem)
        : FormId(aFormId)
        , Item(std::move(aItem))
    {
    }

    // Form id of the reference the engine placed in the world.
    uint32_t FormId{};
    Inventory::Entry Item{};
};
