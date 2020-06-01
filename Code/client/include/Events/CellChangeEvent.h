#pragma once

struct CellChangeEvent
{
    explicit CellChangeEvent(const uint32_t aCellId)
        : CellId(aCellId)
    {}

    uint32_t CellId;
};
