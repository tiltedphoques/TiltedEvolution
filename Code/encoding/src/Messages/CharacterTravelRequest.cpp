#include <Messages/CharacterTravelRequest.h>

void CharacterTravelRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
}

void CharacterTravelRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
}
