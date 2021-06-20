#pragma once

struct InventoryChangeEvent
{
    InventoryChangeEvent(uint32_t aFormId) 
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
