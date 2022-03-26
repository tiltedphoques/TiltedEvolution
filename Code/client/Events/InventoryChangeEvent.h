#pragma once

#include <Structs/Inventory.h>

struct InventoryChangeEvent
{
    InventoryChangeEvent(const uint32_t aFormId, Inventory::Entry&& arItem) 
        : FormId(aFormId), Item(std::move(arItem))
    {
        spdlog::warn("InventoryChangeEvent form: {:X}, item: {:X}", aFormId, arItem.BaseId.BaseId);
    }

    uint32_t FormId{};
    Inventory::Entry Item{};
};
