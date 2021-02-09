#pragma once

#include <Structs/GameId.h>

#include "Message.h"

struct RequestActorMaxValueChanges final : ClientMessage
{
    RequestActorMaxValueChanges() : ClientMessage(kRequestActorMaxValueChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestActorMaxValueChanges& acRhs) const noexcept
    {
        return m_Id == acRhs.m_Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t m_Id;
    Map<uint32_t, float> m_values;
};
