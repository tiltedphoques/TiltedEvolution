#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Factions.h>

struct CacheComponent
{
    CacheComponent() = default;

    Factions FactionsContent{};
};
