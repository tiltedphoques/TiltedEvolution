#pragma once

#include "Message.h"

struct NotifyDeathStateChange final : ServerMessage
{
    NotifyDeathStateChange() : ServerMessage(kNotifyDeathStateChange)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDeathStateChange& acRhs) const noexcept
    {
        return Id == acRhs.Id && IsDead == acRhs.IsDead && GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    bool IsDead;
};
