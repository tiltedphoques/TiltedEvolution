#pragma once
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct CharacterExteriorCellChangeEvent
{
    CharacterExteriorCellChangeEvent(const entt::entity aOwner, const entt::entity aEntity,
                                     const GameId aWorldSpaceId, const GridCellCoords aCurrentCoords)
        : Owner(aOwner)
        , Entity{aEntity}
        , WorldSpaceId(aWorldSpaceId)
        , CurrentCoords(aCurrentCoords)
    {}

    entt::entity Owner;
    entt::entity Entity;
    GameId WorldSpaceId;
    GridCellCoords CurrentCoords;
};
