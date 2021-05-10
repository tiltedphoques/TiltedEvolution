#pragma once
#include "Structs/GameId.h"

struct CharacterInteriorCellChangeEvent
{
    CharacterInteriorCellChangeEvent(const entt::entity aOwner, const entt::entity aEntity, const GameId& acOldCell,
                             const GameId& acNewCell)
        : Owner(aOwner)
        , Entity{aEntity}
        , OldCell(acOldCell)
        , NewCell(acNewCell)
    {}

    entt::entity Owner;
    entt::entity Entity;
    GameId OldCell;
    GameId NewCell;
};
