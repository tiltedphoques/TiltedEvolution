#pragma once

struct Player;
struct PlayerEnterWorldEvent
{
    PlayerEnterWorldEvent(const Player* apPlayer) : pPlayer{apPlayer}
    {}

    const Player* pPlayer;
};
