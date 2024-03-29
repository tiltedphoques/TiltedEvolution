#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/GameId.h>

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
        return GetOpcode() == acRhs.GetOpcode() && Position == acRhs.Position && WorldSpaceFormID == acRhs.WorldSpaceFormID;
    }

    Vector3_NetQuantize Position;
    GameId WorldSpaceFormID;
};
