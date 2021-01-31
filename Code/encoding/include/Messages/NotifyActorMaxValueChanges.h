#pragma once

#include "Message.h"

struct NotifyActorMaxValueChanges final : ServerMessage
{
    NotifyActorMaxValueChanges() : ServerMessage(kNotifyActorMaxValueChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActorMaxValueChanges& acRhs) const noexcept
    {
        return m_Id == acRhs.m_Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_Id;
    Map<uint32_t, float> m_values;
};
