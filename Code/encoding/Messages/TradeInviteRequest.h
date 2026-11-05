#pragma once

#include "Message.h"

struct TradeInviteRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeInviteRequest;

    TradeInviteRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t TargetPlayerId{};
};
