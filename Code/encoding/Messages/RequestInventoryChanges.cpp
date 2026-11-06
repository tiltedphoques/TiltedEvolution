#include <Messages/RequestInventoryChanges.h>
#include <TiltedCore/Serialization.hpp>

void RequestInventoryChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    Item.Serialize(aWriter);
    Serialization::WriteBool(aWriter, Drop);
    Serialization::WriteVarInt(aWriter, DropId);
    if (DropId)
        DropPosition.Serialize(aWriter);
    Serialization::WriteBool(aWriter, UpdateClients);
}

void RequestInventoryChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Item.Deserialize(aReader);
    Drop = Serialization::ReadBool(aReader);
    DropId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    if (DropId)
        DropPosition.Deserialize(aReader);
    UpdateClients = Serialization::ReadBool(aReader);
}
