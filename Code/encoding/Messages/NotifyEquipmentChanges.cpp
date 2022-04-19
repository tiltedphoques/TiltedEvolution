#include <Messages/NotifyEquipmentChanges.h>
#include <TiltedCore/Serialization.hpp>

void NotifyEquipmentChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    ItemId.Serialize(aWriter);
    EquipSlotId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Count);
    Serialization::WriteBool(aWriter, Unequip);
    Serialization::WriteBool(aWriter, IsSpell);
    Serialization::WriteBool(aWriter, IsShout);
}

void NotifyEquipmentChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ItemId.Deserialize(aReader);
    EquipSlotId.Deserialize(aReader);
    Count = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Unequip = Serialization::ReadBool(aReader);
    IsSpell = Serialization::ReadBool(aReader);
    IsShout = Serialization::ReadBool(aReader);
}
