#pragma once

struct Player;
struct PlayerLeaveEvent
{
    PlayerLeaveEvent(Player* apPlayer) : pPlayer{apPlayer}
    {
    }

    Player* pPlayer;
};
