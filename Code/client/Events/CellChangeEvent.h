#pragma once

#include <Structs/GridCellCoords.h>

/**
* Dispatched when the local player moves to a different cell,
* whether that is an exterior or interior cell.
*/
struct CellChangeEvent
{
    GameId CellId{};
    GameId WorldSpaceId{};
    GridCellCoords CurrentCoords{};
};
