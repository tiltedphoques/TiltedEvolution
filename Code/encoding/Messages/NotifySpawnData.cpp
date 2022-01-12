#include <Messages/NotifySpawnData.h>

void NotifySpawnData::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    InitialActorValues.Serialize(aWriter);
    InitialInventory.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsDead);
    Serialization::WriteBool(aWriter, IsWeaponDrawn);
}

void NotifySpawnData::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    InitialActorValues.Deserialize(aReader);
    InitialInventory.Deserialize(aReader);
    IsDead = Serialization::ReadBool(aReader);
    IsWeaponDrawn = Serialization::ReadBool(aReader);
}
