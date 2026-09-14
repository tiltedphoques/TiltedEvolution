#pragma once

#include "Message.h"

struct RequestActorValueChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestActorValueChanges;

    RequestActorValueChanges()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestActorValueChanges& acRhs) const noexcept { return Id == acRhs.Id && OwnershipEpoch == acRhs.OwnershipEpoch && Values == acRhs.Values && GetOpcode() == acRhs.GetOpcode(); }

    uint32_t Id;
    uint32_t OwnershipEpoch{};
    TiltedPhoques::Map<uint32_t, float> Values;
};
