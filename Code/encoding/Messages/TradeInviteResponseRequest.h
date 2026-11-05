#pragma once

#include "Message.h"

struct TradeInviteResponseRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeInviteResponseRequest;

    TradeInviteResponseRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t RequesterPlayerId{};
    bool Accept{false};
};
