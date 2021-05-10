#pragma once

#include <Structs/GridCellCoords.h>

struct CellChangeEvent
{
    GameId CellId{};
    GameId WorldSpaceId{};
    GridCellCoords CurrentCoords{};
};
