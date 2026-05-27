#pragma once

#include "Message.h"

struct PartyAutoJoinToggleRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyAutoJoinToggleRequest;

    PartyAutoJoinToggleRequest()
        : ClientMessage(Opcode)
    {
    }

    virtual ~PartyAutoJoinToggleRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyAutoJoinToggleRequest& achRhs) const noexcept { return GetOpcode() == achRhs.GetOpcode(); }
};
