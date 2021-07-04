#pragma once

#include "Message.h"

struct RequestFireProjectile final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestFireProjectile;

    RequestFireProjectile() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestFireProjectile& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
};
