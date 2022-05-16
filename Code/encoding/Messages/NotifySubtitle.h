#pragma once

#include "Message.h"

struct NotifySubtitle final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySubtitle;

    NotifySubtitle() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySubtitle& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               ServerId == achRhs.ServerId &&
               Text == achRhs.Text;
    }

    uint32_t ServerId{};
    TiltedPhoques::String Text{};
};
