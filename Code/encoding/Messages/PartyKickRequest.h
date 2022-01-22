#pragma once

#include "Message.h"

struct PartyKickRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyKickRequest;

    PartyKickRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~PartyKickRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyKickRequest& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
        PartyMemberPlayerId == achRhs.PartyMemberPlayerId;
    }

    uint32_t PartyMemberPlayerId{};
};
