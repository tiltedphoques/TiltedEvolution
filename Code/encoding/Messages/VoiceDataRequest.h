#pragma once

#include "Message.h"
#include <Structs/VoiceFrame.h>

// The local player's voice, sent unreliably, the server relays it to players in range
struct VoiceDataRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kVoiceDataRequest;

    VoiceDataRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const VoiceDataRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Frame == acRhs.Frame; }

    VoiceFrame Frame{};
};
