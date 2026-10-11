#pragma once

#include "Message.h"

struct NotifyTradeInvite final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTradeInvite;

    NotifyTradeInvite()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t InviterPlayerId{};
    uint64_t ExpiryTick{};
};
