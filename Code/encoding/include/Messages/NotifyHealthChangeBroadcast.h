#pragma once

#include "Message.h"

struct NotifyHealthChangeBroadcast final : ServerMessage
{
    NotifyHealthChangeBroadcast() : ServerMessage(kNotifyHealthChangeBroadcast)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyHealthChangeBroadcast& acRhs) const noexcept
    {
        return m_Id == acRhs.m_Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_Id;
    float m_DeltaHealth;
};
