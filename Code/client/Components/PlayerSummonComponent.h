#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct PlayerSummonComponent
{
    PlayerSummonComponent(uint32_t aId) noexcept : OwnerServerId(aId)
    {
    }

    uint32_t OwnerServerId;
};
