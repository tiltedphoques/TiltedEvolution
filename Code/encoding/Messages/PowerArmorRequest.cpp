#include <Messages/PowerArmorRequest.h>

void PowerArmorRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    FurnitureId.Serialize(aWriter);
    FurnitureBaseId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    Data.Serialize(aWriter);
}

void PowerArmorRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    FurnitureId.Deserialize(aReader);
    FurnitureBaseId.Deserialize(aReader);
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Data.Deserialize(aReader);
}
