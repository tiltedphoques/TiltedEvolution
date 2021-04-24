#pragma once

#include "Message.h"

struct PartyLeaveRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyLeaveRequest;

    PartyLeaveRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~PartyLeaveRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyLeaveRequest& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode();
    }
};
