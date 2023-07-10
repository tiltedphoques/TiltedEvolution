#pragma once

#include "Message.h"

struct RequestDeleteWaypoint final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestDeleteWaypoint;

    RequestDeleteWaypoint() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestDeleteWaypoint() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestDeleteWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

};
