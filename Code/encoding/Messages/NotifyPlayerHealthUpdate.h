#pragma once

#include "Message.h"

struct NotifyPlayerHealthUpdate final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerHealthUpdate;

    NotifyPlayerHealthUpdate() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerHealthUpdate& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId &&
               Percentage == acRhs.Percentage;
    }
    
    uint32_t PlayerId{};
    float Percentage{};
};
