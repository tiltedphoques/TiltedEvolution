#include <Messages/NotifySpawnData.h>
#include <Serialization.hpp>

void NotifySpawnData::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    InitialActorValues.Serialize(aWriter);
    InitialInventory.Serialize(aWriter);
}

void NotifySpawnData::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader);
    InitialActorValues.Deserialize(aReader);
    InitialInventory.Deserialize(aReader);
}
