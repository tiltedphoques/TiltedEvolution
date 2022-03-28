#include <Messages/NotifyEquipmentChanges.h>
#include <TiltedCore/Serialization.hpp>

void NotifyEquipmentChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    EquippedItem.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsLeft);
    Serialization::WriteBool(aWriter, IsSpell);
    Serialization::WriteBool(aWriter, IsShout);
}

void NotifyEquipmentChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    EquippedItem.Deserialize(aReader);
    IsLeft == Serialization::ReadBool(aReader);
    IsSpell == Serialization::ReadBool(aReader);
    IsShout == Serialization::ReadBool(aReader);
}
