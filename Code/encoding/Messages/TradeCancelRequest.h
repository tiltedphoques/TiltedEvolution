#pragma once

#include "Message.h"

struct TradeCancelRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeCancelRequest;

    TradeCancelRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;
};
