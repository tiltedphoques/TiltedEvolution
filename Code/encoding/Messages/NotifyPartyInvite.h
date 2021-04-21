#pragma once

#include "Message.h"

struct NotifyPartyInvite final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPartyInvite;

    NotifyPartyInvite() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPartyInvite() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyInvite& acRhs) const noexcept
    {
        return InviterId == acRhs.InviterId &&
            ExpiryTick == acRhs.ExpiryTick &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t InviterId{};
    uint64_t ExpiryTick{};
};
