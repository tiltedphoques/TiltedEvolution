#pragma once

#include "Message.h"

struct NotifyInterruptCast final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyInterruptCast;

    NotifyInterruptCast() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyInterruptCast& acRhs) const noexcept
    {
        return CasterId == acRhs.CasterId && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t CasterId;
};
