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
        return m_formId == acRhs.m_formId && 
            m_health == acRhs.m_health &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_formId;
    uint32_t m_health;
};
