#pragma once

#include "Message.h"

struct PartyAcceptInviteRequest final : ClientMessage
{
    PartyAcceptInviteRequest() : ClientMessage(kPartyAcceptInviteRequest)
    {
    }

    virtual ~PartyAcceptInviteRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyAcceptInviteRequest& achRhs) const noexcept
    {
        return InviterId == achRhs.InviterId &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t InviterId{};
};
