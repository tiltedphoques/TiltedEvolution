#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct OwnerComponent
{
    OwnerComponent(const ConnectionId_t aConnectionId)
        : ConnectionId(aConnectionId)
    {}

    ConnectionId_t ConnectionId;
    Vector<ConnectionId_t> InvalidOwners{};
};
