#pragma once

#include "Message.h"

struct RequestSpawnData final : ClientMessage
{
    RequestSpawnData() : ClientMessage(kRequestSpawnData)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestSpawnData& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
};
