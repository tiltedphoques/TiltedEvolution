#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/GameId.h>

struct FormIdComponent
{
    FormIdComponent() noexcept
        : FormIdComponent(0,0)
    {}

    FormIdComponent(const GameId& acId) noexcept
        : Id(acId)
    {}

    FormIdComponent(const uint32_t aBaseId, const uint32_t aModId) noexcept
        : Id(aModId, aBaseId)
    {}

    bool operator==(const FormIdComponent& acRhs) const noexcept
    {
        return Id == acRhs.Id;
    }

    bool operator!=(const FormIdComponent& acRhs) const noexcept
    {
        return !operator==(acRhs);
    }

    operator bool() const noexcept
    {
        return Id != GameId{};
    }

    GameId Id;
};
