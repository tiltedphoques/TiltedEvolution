#pragma once

#include <Structs/GridCellCoords.h>

struct GridCellChangeEvent
{
    GridCellChangeEvent(const uint32_t aWorldSpaceId)
        : WorldSpaceId(aWorldSpaceId)
    {}

    uint32_t WorldSpaceId;
    GameId PlayerCell;
    Vector<GameId> Cells{};
    GridCellCoords CenterCoords;
    GridCellCoords PlayerCoords;
};
