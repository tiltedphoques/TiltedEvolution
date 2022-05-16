#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct PlayerComponent
{
    PlayerComponent(uint32_t aId)
        : Id(aId)
    {
    }

    uint32_t Id;
};

