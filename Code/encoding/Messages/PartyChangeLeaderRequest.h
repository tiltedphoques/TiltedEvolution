#pragma once

#include "Message.h"

struct PartyChangeLeaderRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyChangeLeaderRequest;

    PartyChangeLeaderRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~PartyChangeLeaderRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyChangeLeaderRequest& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
        PartyMemberPlayerId == achRhs.PartyMemberPlayerId;
    }

    uint32_t PartyMemberPlayerId{};
};
