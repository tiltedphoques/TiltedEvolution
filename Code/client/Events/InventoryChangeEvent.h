#pragma once

#include <Structs/Inventory.h>
#include <Games/ExtraDataList.h>

struct InventoryChangeEvent
{
    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem) 
        : FormId(aFormId), Item(std::move(arItem))
    {
    }

    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem, bool aDropOrPickUp) 
        : FormId(aFormId), Item(std::move(arItem)), DropOrPickUp(aDropOrPickUp)
    {
    }

    uint32_t FormId{};
    Inventory::Entry Item{};
    bool DropOrPickUp = false;
};