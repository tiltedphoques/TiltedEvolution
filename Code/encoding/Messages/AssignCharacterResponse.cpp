#include <Messages/AssignCharacterResponse.h>

void AssignCharacterResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Cookie);
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteVarInt(aWriter, PlayerId);
    Position.Serialize(aWriter);
    CellId.Serialize(aWriter);
    WorldSpaceId.Serialize(aWriter);
    AllActorValues.Serialize(aWriter);
    CurrentInventory.Serialize(aWriter);
    Serialization::WriteBool(aWriter, Owner);
    Serialization::WriteBool(aWriter, IsDead);
    Serialization::WriteBool(aWriter, IsWeaponDrawn);
}

void AssignCharacterResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Cookie = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Position.Deserialize(aReader);
    CellId.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
    AllActorValues.Deserialize(aReader);
    CurrentInventory.Deserialize(aReader);
    Owner = Serialization::ReadBool(aReader);
    IsDead = Serialization::ReadBool(aReader);
    IsWeaponDrawn = Serialization::ReadBool(aReader);
}
