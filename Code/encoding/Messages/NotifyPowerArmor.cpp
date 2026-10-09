#include <Messages/NotifyPowerArmor.h>

void NotifyPowerArmor::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    FurnitureId.Serialize(aWriter);
    FurnitureBaseId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    Data.Serialize(aWriter);
}

void NotifyPowerArmor::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    FurnitureId.Deserialize(aReader);
    FurnitureBaseId.Deserialize(aReader);
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Data.Deserialize(aReader);
}
