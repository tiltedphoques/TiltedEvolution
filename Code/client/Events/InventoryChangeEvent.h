#pragma once

#include <Structs/Inventory.h>
#include <Games/ExtraDataList.h>

struct InventoryChangeEvent
{
    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem) 
        : FormId(aFormId), Item(std::move(arItem))
    {
        spdlog::warn("InventoryChangeEvent form: {:X}, item: {:X}", aFormId, arItem.BaseId.BaseId);
    }

    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem, bool aDropOrPickUp) 
        : FormId(aFormId), Item(std::move(arItem)), DropOrPickUp(aDropOrPickUp)
    {
        spdlog::warn("InventoryChangeEvent form: {:X}, item: {:X}, DropOrPickUp: {}", aFormId, arItem.BaseId.BaseId, aDropOrPickUp);
    }

    uint32_t FormId{};
    Inventory::Entry Item{};
    bool DropOrPickUp = false;
};
