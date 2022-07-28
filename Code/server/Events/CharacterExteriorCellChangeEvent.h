#pragma once
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct Player;

/**
* @brief Dispatched when a player enters a new exterior cell.
*/
struct CharacterExteriorCellChangeEvent
{
    CharacterExteriorCellChangeEvent(Player* apOwner, const entt::entity aEntity,
                                     const GameId aWorldSpaceId, const GridCellCoords aCurrentCoords)
        : Owner(apOwner)
        , Entity{aEntity}
        , WorldSpaceId(aWorldSpaceId)
        , CurrentCoords(aCurrentCoords)
    {}

    Player* Owner;
    entt::entity Entity;
    GameId WorldSpaceId;
    GridCellCoords CurrentCoords;
};
