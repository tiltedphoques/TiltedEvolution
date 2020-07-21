#pragma once

#include "Message.h"
#include <Buffer.hpp>

struct RemoveCharacterRequest final : ClientMessage
{
    RemoveCharacterRequest()
        : ClientMessage(kRemoveCharacterRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RemoveCharacterRequest& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
};
