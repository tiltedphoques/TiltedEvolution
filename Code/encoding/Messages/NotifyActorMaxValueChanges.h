#pragma once

#include "Message.h"


struct NotifyActorMaxValueChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyActorMaxValueChanges;

    NotifyActorMaxValueChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActorMaxValueChanges& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               Values == acRhs.Values &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    TiltedPhoques::Map<uint32_t, float> Values;
};
