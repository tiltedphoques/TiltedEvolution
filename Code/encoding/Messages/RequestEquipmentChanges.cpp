#include <Messages/RequestEquipmentChanges.h>
#include <TiltedCore/Serialization.hpp>

void RequestEquipmentChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    CurrentEquipment.Serialize(aWriter);
}

void RequestEquipmentChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CurrentEquipment.Deserialize(aReader);
}
