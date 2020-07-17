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
        : BaseId(acId.BaseId)
        , ModId(acId.ModId)
    {}

    FormIdComponent(const uint32_t aBaseId, const uint32_t aModId) noexcept
        : BaseId(aBaseId)
        , ModId(aModId)
    {}

    bool operator==(const FormIdComponent& acRhs) const noexcept
    {
        return BaseId == acRhs.BaseId && ModId == acRhs.ModId;
    }

    bool operator!=(const FormIdComponent& acRhs) const noexcept
    {
        return !operator==(acRhs);
    }

    operator bool() const noexcept
    {
        return BaseId != 0 || ModId != 0;
    }

    uint32_t BaseId;
    uint32_t ModId;
};
