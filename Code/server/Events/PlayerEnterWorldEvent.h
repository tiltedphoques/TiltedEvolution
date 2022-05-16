#pragma once

struct Player;

/**
* @brief Dispatched when a player enters joins server and requests its actor to be loaded.
*/
struct PlayerEnterWorldEvent
{
    PlayerEnterWorldEvent(const Player* apPlayer) : pPlayer{apPlayer}
    {}

    const Player* pPlayer;
};
