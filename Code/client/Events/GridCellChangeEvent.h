#pragma once

#include <Structs/GridCellCoords.h>

struct GridCellChangeEvent
{
    uint32_t WorldSpaceId;
    GameId PlayerCell;
    Vector<GameId> Cells{};
    GridCellCoords CenterCoords;
    GridCellCoords PlayerCoords;
};
