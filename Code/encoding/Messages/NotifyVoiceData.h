#pragma once

#include "Message.h"
#include <Structs/VoiceFrame.h>

// Another player's voice, relayed by the server
struct NotifyVoiceData final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyVoiceData;

    NotifyVoiceData()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyVoiceData& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && PlayerId == acRhs.PlayerId && Frame == acRhs.Frame; }

    uint32_t PlayerId{};
    VoiceFrame Frame{};
};
