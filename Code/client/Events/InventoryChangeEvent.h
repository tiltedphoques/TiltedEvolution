#pragma once

#include <Structs/Inventory.h>
#include <ExtraData/ExtraDataList.h>

/**
* @brief Dispatched when the contents of an object or actor inventory changes locally.
* 
* The event has a Drop member variable, since dropped items need to be handled differently.
*/
struct InventoryChangeEvent
{
    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem) 
        : FormId(aFormId), Item(std::move(arItem))
    {
    }

    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem, bool aDrop) 
        : FormId(aFormId), Item(std::move(arItem)), Drop(aDrop)
    {
    }

    uint32_t FormId{};
    Inventory::Entry Item{};
    bool Drop = false;
};
