#pragma once

#include "Message.h"

struct RequestPlayerHealthUpdate final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestPlayerHealthUpdate;

    RequestPlayerHealthUpdate() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestPlayerHealthUpdate() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestPlayerHealthUpdate& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && 
               Percentage == acRhs.Percentage;
    }
    
    float Percentage{};
};
