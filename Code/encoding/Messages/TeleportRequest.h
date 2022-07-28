#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct TeleportRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTeleportRequest;

    TeleportRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const TeleportRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId;
    }

    uint16_t PlayerId{};
};
