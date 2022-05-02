#pragma once

#include "Message.h"

struct NotifyPlayerRespawn final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerRespawn;

    NotifyPlayerRespawn() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerRespawn& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && 
               GoldLost == acRhs.GoldLost;
    }

    int32_t GoldLost{};
};
