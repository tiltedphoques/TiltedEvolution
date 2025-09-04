#include <Messages/CharacterSpawnRequest.h>

void CharacterSpawnRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    FormId.Serialize(aWriter);
    BaseId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
    Rotation.Serialize(aWriter);
    aWriter.WriteBits(ChangeFlags, 32);
    Serialization::WriteString(aWriter, AppearanceBuffer);
    InventoryContent.Serialize(aWriter);
    FactionsContent.Serialize(aWriter);

    // TODO: Maybe just the EventNames is enough? Why send whole ActionEvents?
    aWriter.WriteBits(ActionsToReplay.size() & 0xFF, 8);
    ActionEvent latestSerializedAction{};
    for (int i = 0; i < ActionsToReplay.size(); ++i)
    {
        ActionsToReplay[i].GenerateDifferential(latestSerializedAction, aWriter);
        latestSerializedAction = ActionsToReplay[i];
    }
    FaceTints.Serialize(aWriter);
    InitialActorValues.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, PlayerId);
    Serialization::WriteBool(aWriter, IsDead);
    Serialization::WriteBool(aWriter, IsPlayer);
    Serialization::WriteBool(aWriter, IsWeaponDrawn);
    Serialization::WriteBool(aWriter, IsPlayerSummon);
}

void CharacterSpawnRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    FormId.Deserialize(aReader);
    BaseId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
    Rotation.Deserialize(aReader);

    uint64_t dest = 0;
    aReader.ReadBits(dest, 32);
    ChangeFlags = dest & 0xFFFFFFFF;

    AppearanceBuffer = Serialization::ReadString(aReader);
    InventoryContent = {};
    InventoryContent.Deserialize(aReader);

    FactionsContent = {};
    FactionsContent.Deserialize(aReader);

    uint64_t actionsToReplayCount = 0;
    aReader.ReadBits(actionsToReplayCount, 8);
    ActionsToReplay.resize(actionsToReplayCount);
    for (ActionEvent& replayAction : ActionsToReplay)
        replayAction.ApplyDifferential(aReader);

    FaceTints.Deserialize(aReader);
    InitialActorValues.Deserialize(aReader);
    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    IsDead = Serialization::ReadBool(aReader);
    IsPlayer = Serialization::ReadBool(aReader);
    IsWeaponDrawn = Serialization::ReadBool(aReader);
    IsPlayerSummon = Serialization::ReadBool(aReader);
}
