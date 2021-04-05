#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct DeathComponent
{
    bool IsDead{};
    bool RequestResurrect{};
};
