#include <Messages/NotifyPlayerLevel.h>

void NotifyPlayerLevel::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PlayerId);
    Serialization::WriteVarInt(aWriter, NewLevel);
}

void NotifyPlayerLevel::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    NewLevel = Serialization::ReadVarInt(aReader) & 0xFFFF;
}
