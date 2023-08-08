#pragma once

#include "Message.h"

struct NotifyRemoveWaypoint final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyRemoveWaypoint;

    NotifyRemoveWaypoint() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRemoveWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }
};
