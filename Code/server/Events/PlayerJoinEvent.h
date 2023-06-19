#pragma once

#include <Structs/GameId.h>
#include <Structs/TimeModel.h>
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

    PlayerJoinEvent(Player* apPlayer, GameId aWorldSpaceId, GameId aCellId, TimeModel aTimeModel)
        : pPlayer(apPlayer)
        , WorldSpaceId(aWorldSpaceId)
        , CellId(aCellId)
        , PlayerTime(aTimeModel)
    {
    }

    Player* pPlayer;
    GameId WorldSpaceId{};
    GameId CellId{};
    TimeModel PlayerTime;
};
