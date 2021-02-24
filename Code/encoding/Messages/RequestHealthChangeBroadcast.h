#pragma once

#include "Message.h"

struct RequestHealthChangeBroadcast final : ClientMessage
{
    RequestHealthChangeBroadcast() : ClientMessage(kRequestHealthChangeBroadcast)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestHealthChangeBroadcast& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               DeltaHealth == acRhs.DeltaHealth &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    float DeltaHealth;
};
