#pragma once

#include "Message.h"

struct RequestActorMaxValueChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestActorMaxValueChanges;

    RequestActorMaxValueChanges() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestActorMaxValueChanges& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               Values == acRhs.Values &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    TiltedPhoques::Map<uint32_t, float> Values;
};
