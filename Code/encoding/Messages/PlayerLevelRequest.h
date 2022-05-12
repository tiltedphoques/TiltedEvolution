#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct PlayerLevelRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPlayerLevelRequest;

    PlayerLevelRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PlayerLevelRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               NewLevel == acRhs.NewLevel;
    }

    uint16_t NewLevel{};
};
