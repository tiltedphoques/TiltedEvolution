#pragma once

#include "Message.h"

struct RequestRemoveWaypoint final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestRemoveWaypoint;

    RequestRemoveWaypoint() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestRemoveWaypoint() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestRemoveWaypoint& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }
};
