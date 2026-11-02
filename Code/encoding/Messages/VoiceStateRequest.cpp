#include <Messages/VoiceStateRequest.h>

void VoiceStateRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Listening);
    aWriter.WriteBits(ListenChannel, 8);
}

void VoiceStateRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Listening = Serialization::ReadBool(aReader);
    uint64_t channel = 0;
    aReader.ReadBits(channel, 8);
    ListenChannel = channel < kChannelCount ? static_cast<Channel>(channel) : kEveryone;
}
