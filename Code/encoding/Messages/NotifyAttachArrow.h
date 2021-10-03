#pragma once

#include "Message.h"

struct NotifyAttachArrow final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyAttachArrow;

    NotifyAttachArrow() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyAttachArrow& acRhs) const noexcept
    {
        return ShooterId == acRhs.ShooterId && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t ShooterId;
};
