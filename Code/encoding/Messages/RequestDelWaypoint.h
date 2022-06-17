#pragma once

#include "Message.h"

struct RequestDelWaypoint final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestDelWaypoint;

    RequestDelWaypoint() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestDelWaypoint() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestDelWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

};
