#pragma once

#include "Structs/GameId.h"

/**
* @brief Dispatched when a player leaves a certain cell, be it exterior or interior.
*/
struct PlayerLeaveCellEvent
{
    PlayerLeaveCellEvent(const GameId aOldCell)
        : OldCell(aOldCell)
    {}

    GameId OldCell;
};
