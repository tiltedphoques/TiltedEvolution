#pragma once

#include "Message.h"

// Which voices the local player wants, so the server only relays what will be played
struct VoiceStateRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kVoiceStateRequest;

    enum Channel : uint8_t
    {
        kEveryone = 0,
        kParty,
        kStrangers,
        kChannelCount
    };

    VoiceStateRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const VoiceStateRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Listening == acRhs.Listening && ListenChannel == acRhs.ListenChannel; }

    // False while the player has voip off, nothing is relayed to them
    bool Listening{};
    Channel ListenChannel{kEveryone};
};
