#pragma once

#include "Message.h"

struct PartyCreateRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPartyCreateRequest;

    PartyCreateRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~PartyCreateRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PartyCreateRequest& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode();
    }
};
