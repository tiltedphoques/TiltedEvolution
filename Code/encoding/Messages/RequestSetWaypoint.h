#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>

struct RequestSetWaypoint final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestSetWaypoint;

    RequestSetWaypoint() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestSetWaypoint() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestSetWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && Position == acRhs.Position;
    }

    Vector3_NetQuantize Position;
};
