#pragma once
#include "Structs/GameId.h"

struct Player;

/**
* @brief Dispatched when a player enters a new interior cell.
*/
struct CharacterInteriorCellChangeEvent
{
    CharacterInteriorCellChangeEvent(Player* apOwner, const entt::entity aEntity, const GameId& acNewCell)
        : Owner(apOwner)
        , Entity{aEntity}
        , NewCell(acNewCell)
    {}

    Player* Owner;
    entt::entity Entity;
    GameId NewCell;
};
