#pragma once

#include "Message.h"

enum class TradeCancelReason : uint8_t
{
    Declined = 0,
    Cancelled,
    PartnerBusy,
    SelfBusy,
    PlayerLeft,
    Timeout,
    FailedValidation
};

struct NotifyTradeCancel final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTradeCancel;

    NotifyTradeCancel()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t PartnerPlayerId{};
    TradeCancelReason Reason{TradeCancelReason::Cancelled};
    bool WasInitiator{false};
};
