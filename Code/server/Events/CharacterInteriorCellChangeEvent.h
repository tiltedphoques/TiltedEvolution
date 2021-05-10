#pragma once
#include "Structs/GameId.h"

struct CharacterInteriorCellChangeEvent
{
    CharacterInteriorCellChangeEvent(const entt::entity aOwner, const entt::entity aEntity, const GameId& acNewCell)
        : Owner(aOwner)
        , Entity{aEntity}
        , NewCell(acNewCell)
    {}

    entt::entity Owner;
    entt::entity Entity;
    GameId NewCell;
};
