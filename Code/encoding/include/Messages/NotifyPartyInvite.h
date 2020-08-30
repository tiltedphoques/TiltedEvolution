#pragma once

#include "Message.h"

struct NotifyPartyInvite final : ServerMessage
{
    NotifyPartyInvite() : 
        ServerMessage(kNotifyPartyInvite)
    {
    }

    virtual ~NotifyPartyInvite() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyInvite& acRhs) const noexcept
    {
        return InviterId == acRhs.InviterId &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t InviterId{};
};
