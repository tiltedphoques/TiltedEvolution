#pragma once
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct CharacterGridCellShiftEvent
{
    CharacterGridCellShiftEvent(const entt::entity aOwner, const entt::entity aEntity, const GameId aWorldSpaceId,
                                const GridCellCoords aPlayerCoords, const Vector<GameId> aCells)
        : Owner(aOwner)
        , Entity{aEntity}
        , WorldSpaceId(aWorldSpaceId)
        , PlayerCoords(aPlayerCoords)
        , Cells(aCells)
    {}

    entt::entity Owner;
    entt::entity Entity;
    GameId WorldSpaceId;
    GridCellCoords PlayerCoords;
    Vector<GameId> Cells;
};
