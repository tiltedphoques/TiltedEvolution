#pragma once

using TiltedPhoques::Buffer;

// One 20 ms Opus packet of a player's voice
struct VoiceFrame
{
    // Opus packets for 20 ms of speech at our bitrate are well under this, anything bigger is dropped
    static constexpr size_t kMaxSize = 512;

    bool operator==(const VoiceFrame& acRhs) const noexcept { return Sequence == acRhs.Sequence && Data == acRhs.Data; }
    bool operator!=(const VoiceFrame& acRhs) const noexcept { return !this->operator==(acRhs); }

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // False when the frame is empty or too big to be a voice packet
    bool Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    // Counts up per frame the speaker sends, receivers use it to spot lost and reordered frames
    uint16_t Sequence{};
    TiltedPhoques::Vector<uint8_t> Data{};
};
