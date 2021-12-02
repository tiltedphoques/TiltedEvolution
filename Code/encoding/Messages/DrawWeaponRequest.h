#pragma once

#include "Message.h"

struct DrawWeaponRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kDrawWeaponRequest;

    DrawWeaponRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DrawWeaponRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id &&
               IsWeaponDrawn == acRhs.IsWeaponDrawn && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id{};
    bool IsWeaponDrawn{};
};
