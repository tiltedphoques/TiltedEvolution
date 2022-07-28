#pragma once

#include "Message.h"

struct PlayerRespawnRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPlayerRespawnRequest;

    PlayerRespawnRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PlayerRespawnRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }
};
