#pragma once

struct Player;

/**
* @brief Dispatched when a player joins the server.
*/
struct PlayerJoinEvent
{
    PlayerJoinEvent(Player* apPlayer) : pPlayer{apPlayer}
    {
    }

    PlayerJoinEvent(Player* apPlayer, GameId aWorldSpaceId, GameId aCellId) 
        : pPlayer(apPlayer), WorldSpaceId(aWorldSpaceId), CellId(aCellId)
    {
    }

    Player* pPlayer;
    GameId WorldSpaceId{};
    GameId CellId{};
};
