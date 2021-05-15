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
    LatestAction.GenerateDifferential(ActionEvent{}, aWriter);
    FaceTints.Serialize(aWriter);
    InitialActorValues.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsDead);
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

    LatestAction = ActionEvent{};
    LatestAction.ApplyDifferential(aReader);

    FaceTints.Deserialize(aReader);
    InitialActorValues.Deserialize(aReader);
    IsDead = Serialization::ReadBool(aReader);
}
