#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>


struct NotifySetWaypoint final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySetWaypoint;

    NotifySetWaypoint() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySetWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Position == acRhs.Position;
    }

    Vector3_NetQuantize Position;

    
};
