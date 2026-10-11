#pragma once

#include "Message.h"

struct NotifyTradeComplete final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTradeComplete;

    NotifyTradeComplete()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t PartnerPlayerId{};
};
