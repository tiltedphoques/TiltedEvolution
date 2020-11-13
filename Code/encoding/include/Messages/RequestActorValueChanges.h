#pragma once

#include "Message.h"

struct RequestActorValueChanges final : ClientMessage
{
    RequestActorValueChanges() : ClientMessage(kRequestActorValueChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestActorValueChanges& acRhs) const noexcept
    {
        return m_formId == acRhs.m_formId && m_health == acRhs.m_health && GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_formId;
    uint32_t m_health;
};
