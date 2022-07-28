#pragma once

#include "Message.h"

struct NotifyDialogue final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDialogue;

    NotifyDialogue() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDialogue& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               ServerId == achRhs.ServerId &&
               SoundFilename == achRhs.SoundFilename;
    }

    uint32_t ServerId{};
    TiltedPhoques::String SoundFilename{};
};
