#pragma once

#include "Message.h"

struct NotifyFireProjectile final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyFireProjectile;
    NotifyFireProjectile() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyFireProjectile& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
};
