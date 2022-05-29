#pragma once

#include <Structs/GridCellCoords.h>

/**
* @brief Dispatched when the grid cell shifts in an exterior world space.
*/
struct GridCellChangeEvent
{
    uint32_t WorldSpaceId{};
    GameId PlayerCell{};
    Vector<GameId> Cells{};
    GridCellCoords CenterCoords{};
};
