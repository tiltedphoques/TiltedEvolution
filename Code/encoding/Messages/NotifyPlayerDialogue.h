#pragma once

#include "Message.h"

struct NotifyPlayerDialogue final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerDialogue;

    NotifyPlayerDialogue() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerDialogue& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               Name == achRhs.Name &&
               Text == achRhs.Text;
    }

    TiltedPhoques::String Name{};
    TiltedPhoques::String Text{};
};
