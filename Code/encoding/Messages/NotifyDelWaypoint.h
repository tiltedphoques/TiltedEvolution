#pragma once

#include "Message.h"

struct NotifyDelWaypoint final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDelWaypoint;

    NotifyDelWaypoint() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDelWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

};
