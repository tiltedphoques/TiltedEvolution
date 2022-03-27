#include <Messages/NotifyEquipmentChanges.h>
#include <TiltedCore/Serialization.hpp>

void NotifyEquipmentChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    CurrentEquipment.Serialize(aWriter);
}

void NotifyEquipmentChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CurrentEquipment.Deserialize(aReader);
}
