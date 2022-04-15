#pragma once

#include "Message.h"

struct NotifySyncExperience final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySyncExperience;

    NotifySyncExperience() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySyncExperience& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Experience == acRhs.Experience;
    }

    float Experience{};
};
