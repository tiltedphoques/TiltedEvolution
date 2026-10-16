#include <Messages/NotifyDialogue.h>

void NotifyDialogue::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteString(aWriter, SoundFilename);
    SceneId.Serialize(aWriter);
}

void NotifyDialogue::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SoundFilename = Serialization::ReadString(aReader);
    SceneId.Deserialize(aReader);
}
