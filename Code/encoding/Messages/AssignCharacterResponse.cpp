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

    aWriter.WriteBits(ActionsToReplay.size() & 0xFF, 8);
    for (size_t i = 0; i < ActionsToReplay.size(); ++i)
    {
        ActionsToReplay[i].GenerateDifferential(ActionEvent{}, aWriter);
    }
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

    uint64_t actionsToReplayCount = 0;
    aReader.ReadBits(actionsToReplayCount, 8);
    ActionsToReplay.resize(actionsToReplayCount);
    for (ActionEvent& replayAction : ActionsToReplay)
    {
        replayAction.ApplyDifferential(aReader);
    }
    Owner = Serialization::ReadBool(aReader);
    IsDead = Serialization::ReadBool(aReader);
    IsWeaponDrawn = Serialization::ReadBool(aReader);
}
