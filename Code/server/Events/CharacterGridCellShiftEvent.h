#pragma once
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct CharacterGridCellShiftEvent
{
    CharacterGridCellShiftEvent(const entt::entity aOwner, const entt::entity aEntity, const GameId aOldWorldSpaceId, 
                                const GameId aNewWorldSpaceId, const GridCellCoords& aOldCoords, const GridCellCoords& aNewCoords)
        : Owner(aOwner)
        , Entity{aEntity}
        , OldWorldSpaceId(aOldWorldSpaceId)
        , NewWorldSpaceId(aNewWorldSpaceId)
        , OldCoords(aOldCoords)
        , NewCoords(aNewCoords)
    {}

    entt::entity Owner;
    entt::entity Entity;
    GameId OldWorldSpaceId;
    GameId NewWorldSpaceId;
    GridCellCoords OldCoords;
    GridCellCoords NewCoords;
};
