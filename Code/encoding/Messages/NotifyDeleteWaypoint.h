#pragma once

#include "Message.h"

struct NotifyDeleteWaypoint final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDeleteWaypoint;

    NotifyDeleteWaypoint() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDeleteWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

};
