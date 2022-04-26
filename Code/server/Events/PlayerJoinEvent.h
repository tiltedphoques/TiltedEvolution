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

    Player* pPlayer;
};
