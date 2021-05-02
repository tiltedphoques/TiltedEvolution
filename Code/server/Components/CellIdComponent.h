#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct CellIdComponent
{
    CellIdComponent(GameId aCellId)
        : Cell(aCellId)
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
    int32_t CurrentGridX{0};
    int32_t CurrentGridY{0};
};
