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

    bool operator==(const NotifySubtitle& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               Text == acRhs.Text &&
               TopicFormId == acRhs.TopicFormId;
    }

    uint32_t ServerId{};
    TiltedPhoques::String Text{};
    uint32_t TopicFormId{};
};
