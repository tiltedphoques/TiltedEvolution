#pragma once

#include "Message.h"
#include <Buffer.hpp>

struct NotifyRemoveCharacter final : ServerMessage
{
    NotifyRemoveCharacter()
        : ServerMessage(kNotifyRemoveCharacter)
    {
    }

    virtual ~NotifyRemoveCharacter() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRemoveCharacter& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
};
