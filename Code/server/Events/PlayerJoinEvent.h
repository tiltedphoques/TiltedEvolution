#pragma once

#include <Structs/GameId.h>
#include <DateTime.h>

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

    PlayerJoinEvent(Player* apPlayer, GameId aWorldSpaceId, GameId aCellId)
        : pPlayer(apPlayer)
        , WorldSpaceId(aWorldSpaceId)
        , CellId(aCellId)
    {
    }

    PlayerJoinEvent(Player* apPlayer, GameId aWorldSpaceId, GameId aCellId, DateTime aDateTime)
        : pPlayer(apPlayer)
        , WorldSpaceId(aWorldSpaceId)
        , CellId(aCellId)
        , PlayerTime(aDateTime)
    {
    }

    Player* pPlayer;
    GameId WorldSpaceId{};
    GameId CellId{};
    DateTime PlayerTime;
};
