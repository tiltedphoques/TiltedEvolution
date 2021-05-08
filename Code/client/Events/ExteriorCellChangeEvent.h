#pragma once

#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct ExteriorCellChangeEvent
{
    GameId WorldSpaceId{};
    GameId CellId{};
    GridCellCoords CurrentCoords{};
};
