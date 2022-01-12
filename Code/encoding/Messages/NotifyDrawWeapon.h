#pragma once

#include "Message.h"

struct NotifyDrawWeapon final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDrawWeapon;

    NotifyDrawWeapon() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDrawWeapon& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               IsWeaponDrawn == acRhs.IsWeaponDrawn &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id{};
    bool IsWeaponDrawn{};
};
