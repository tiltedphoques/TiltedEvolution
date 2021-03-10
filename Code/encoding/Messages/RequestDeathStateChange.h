#pragma once

#include "Message.h"

struct RequestDeathStateChange final : ClientMessage
{
    RequestDeathStateChange() : ClientMessage(kRequestDeathStateChange)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestDeathStateChange& acRhs) const noexcept
    {
        return Id == acRhs.Id && IsDead == acRhs.IsDead && GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    bool IsDead;
};
