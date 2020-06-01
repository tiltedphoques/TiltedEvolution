#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct RemoteComponent
{
    RemoteComponent(uint32_t aId) noexcept : Id(aId) {}

    uint32_t Id;
};
