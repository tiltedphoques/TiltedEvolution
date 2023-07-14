#pragma once

#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct Player;

/**
 * @brief Dispatched when a player joins the server.
 */
struct PlayerJoinEvent
{
    PlayerJoinEvent(Player* apPlayer)
        : pPlayer{apPlayer}
    {
    }

    PlayerJoinEvent(Player* apPlayer, GameId aWorldSpaceId, GameId aCellId, GridCellCoords aCenterCoords) 
        : pPlayer(apPlayer)
        , WorldSpaceId(aWorldSpaceId)
        , CellId(aCellId)
        , CenterCoords(aCenterCoords)
    {
    }

    Player* pPlayer;
    GameId WorldSpaceId{};
    GameId CellId{};
    GridCellCoords CenterCoords{};
};
