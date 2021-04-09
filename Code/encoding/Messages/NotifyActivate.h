#pragma once

#include "Message.h"

struct NotifyActivate final : ServerMessage
{
    NotifyActivate() : ServerMessage(kNotifyActivate)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActivate& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               ActivatorId == acRhs.ActivatorId &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    uint32_t ActivatorId;
};
