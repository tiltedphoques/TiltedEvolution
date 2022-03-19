#pragma once

#include "Message.h"

struct NotifyRespawn final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyRespawn;
    NotifyRespawn() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRespawn& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ActorId == acRhs.ActorId;
    }

    uint32_t ActorId;
};
