#include <Messages/NotifyPlayerJoined.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPlayerJoined::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PlayerId);
    Serialization::WriteString(aWriter, Username);
    WorldSpaceId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Level);
}

void NotifyPlayerJoined::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Username = Serialization::ReadString(aReader);
    WorldSpaceId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    Level = Serialization::ReadVarInt(aReader) & 0xFFFF;
}
