#pragma once

#include "Message.h"

struct RequestOwnershipTransfer final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestOwnershipTransfer;

    RequestOwnershipTransfer() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestOwnershipTransfer() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestOwnershipTransfer& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId && GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
};
