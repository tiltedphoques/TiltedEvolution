#pragma once

#include "Message.h"

struct PartyInviteRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyInviteRequest;

    PartyInviteRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~PartyInviteRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyInviteRequest& achRhs) const noexcept
    {
        return PlayerId == achRhs.PlayerId &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t PlayerId{};
};
