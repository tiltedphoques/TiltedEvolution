#include <Messages/NotifyCharacterTravel.h>

void NotifyCharacterTravel::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
    Serialization::WriteBool(aWriter, Owner);
}

void NotifyCharacterTravel::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
    Owner = Serialization::ReadBool(aReader);
}
