#include <Messages/NotifyVoiceData.h>

void NotifyVoiceData::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PlayerId);
    Frame.Serialize(aWriter);
}

void NotifyVoiceData::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Frame.Deserialize(aReader);
}
