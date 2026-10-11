#pragma once

#include "Message.h"

struct NotifyTradeStarted final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTradeStarted;

    NotifyTradeStarted()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t PartnerPlayerId{};
    bool InitiatedBySelf{false};
};
