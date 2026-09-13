#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct PartyComponent
{
    PartyComponent() {}

    void ExpireInvitations(uint64_t aCurrentTick) noexcept
    {
        auto itor = Invitations.begin();
        while (itor != Invitations.end())
        {
            if (itor->second <= aCurrentTick)
                itor = Invitations.erase(itor);
            else
                ++itor;
        }
    }

    bool TryConsumeInvitation(uint32_t aInviterId, uint64_t aCurrentTick) noexcept
    {
        const auto itor = Invitations.find(aInviterId);
        if (itor == Invitations.end())
            return false;

        const bool isValid = itor->second > aCurrentTick;
        Invitations.erase(itor);
        return isValid;
    }

    std::optional<uint32_t> JoinedPartyId;
    TiltedPhoques::Map<uint32_t, uint64_t> Invitations;
};
