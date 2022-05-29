#pragma once

#include "Message.h"

struct InterruptCastRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kInterruptCastRequest;

    InterruptCastRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const InterruptCastRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               CasterId == acRhs.CasterId && 
               CastingSource == acRhs.CastingSource;
    }

    uint32_t CasterId;
    int32_t CastingSource;
};
