#pragma once

#include "Message.h"

struct NotifyActorValueChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyActorValueChanges;

    NotifyActorValueChanges()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActorValueChanges& acRhs) const noexcept { return Id == acRhs.Id && OwnershipEpoch == acRhs.OwnershipEpoch && Values == acRhs.Values && GetOpcode() == acRhs.GetOpcode(); }

    uint32_t Id;
    uint32_t OwnershipEpoch{};
    TiltedPhoques::Map<uint32_t, float> Values;
};
