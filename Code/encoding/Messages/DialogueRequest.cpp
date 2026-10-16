#include <Messages/DialogueRequest.h>

void DialogueRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteString(aWriter, SoundFilename);
    SceneId.Serialize(aWriter);
}

void DialogueRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SoundFilename = Serialization::ReadString(aReader);
    SceneId.Deserialize(aReader);
}
