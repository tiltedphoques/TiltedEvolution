#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct PartyComponent
{
    PartyComponent() {}

    std::optional<uint32_t> JoinedPartyId;
    Map<uint32_t, uint64_t> Invitations;
};
