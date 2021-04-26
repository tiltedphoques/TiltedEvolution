#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/ActionEvent.h>

struct LocalComponent
{
    LocalComponent(uint32_t aId) noexcept : Id(aId) {}

    uint32_t Id;
    ActionEvent CurrentAction;
    bool IsDead;
};
