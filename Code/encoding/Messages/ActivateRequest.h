#pragma once

#include "Message.h"

struct ActivateRequest final : ClientMessage
{
    ActivateRequest() : ClientMessage(kActivateRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ActivateRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               ActivatorId == acRhs.ActivatorId &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    uint32_t ActivatorId;
};
