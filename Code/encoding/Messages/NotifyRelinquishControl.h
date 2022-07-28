#pragma once

#include "Message.h"

struct NotifyRelinquishControl final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyRelinquishControl;

    NotifyRelinquishControl() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRelinquishControl& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
            ServerId == acRhs.ServerId;
    }

    uint32_t ServerId;
};
