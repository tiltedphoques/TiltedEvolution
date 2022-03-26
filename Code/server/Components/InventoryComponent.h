#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Inventory.h>

struct InventoryComponent
{
    InventoryComponent(Inventory&& aInventory)
        : Content(std::move(aInventory))
    {
    }

    Inventory Content{};
};
