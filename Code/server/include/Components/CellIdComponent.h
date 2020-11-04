#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct CellIdComponent
{
    CellIdComponent(FormIdComponent aCellId)
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

    FormIdComponent Cell;
};
