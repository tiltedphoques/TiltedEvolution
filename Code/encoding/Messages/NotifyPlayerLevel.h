#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct NotifyPlayerLevel final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerLevel;

    NotifyPlayerLevel() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerLevel& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId;
    }

    uint32_t PlayerId{};
    uint16_t NewLevel{};
};
