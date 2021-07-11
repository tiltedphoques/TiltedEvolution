#pragma once

#include "Structs/GameId.h"

struct PlayerLeaveCellEvent
{
    PlayerLeaveCellEvent(const GameId aOldCell)
        : OldCell(aOldCell)
    {}

    GameId OldCell;
};
