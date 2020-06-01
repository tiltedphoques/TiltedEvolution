#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct CellIdComponent
{
    CellIdComponent(FormIdComponent aCellId)
        : CellId(aCellId)
    {}

    bool operator==(const CellIdComponent& acRhs) const noexcept
    {
        return CellId == acRhs.CellId ;
    }

    bool operator!=(const CellIdComponent& acRhs) const noexcept
    {
        return !operator==(acRhs);
    }

    FormIdComponent CellId;
};
