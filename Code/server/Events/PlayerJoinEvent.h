#pragma once

struct Player;

struct PlayerJoinEvent
{
    PlayerJoinEvent(Player* apPlayer) : pPlayer{apPlayer}
    {
    }

    Player* pPlayer;
};
