#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct CellIdComponent
{
    CellIdComponent(GameId aCellId)
        : Cell(aCellId)
    {}

    CellIdComponent(GameId aCellId, GameId aWorldSpaceId) 
        : Cell(aCellId), WorldSpaceId(aWorldSpaceId)
    {}

    bool operator==(const CellIdComponent& acRhs) const noexcept
    {
        return Cell == acRhs.Cell ;
    }

    bool operator!=(const CellIdComponent& acRhs) const noexcept
    {
        return !operator==(acRhs);
    }

    GameId Cell;
    GameId WorldSpaceId{};
};
