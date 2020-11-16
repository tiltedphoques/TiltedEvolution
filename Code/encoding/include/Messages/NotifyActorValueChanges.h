#pragma once

#include "Message.h"

struct NotifyActorValueChanges final : ServerMessage
{
    NotifyActorValueChanges() : ServerMessage(kNotifyActorValueChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActorValueChanges& acRhs) const noexcept
    {
        return m_Id == acRhs.m_Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_Id;
    Map<uint32_t, float> m_values;
};
