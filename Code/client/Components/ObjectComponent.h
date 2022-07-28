#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct ObjectComponent
{
    ObjectComponent(const uint32_t acId)
        : Id(acId)
    {
    }

    uint32_t Id;
};
