#include <Messages/CharacterSpawnRequest.h>

void CharacterSpawnRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    FormId.Serialize(aWriter);
    BaseId.Serialize(aWriter);
    Position.Serialize(aWriter);
    Rotation.Serialize(aWriter);
    Serialization::WriteString(aWriter, AppearanceBuffer);
    Serialization::WriteString(aWriter, InventoryBuffer);
    LatestAction.GenerateDifferential(ActionEvent{}, aWriter);
}

void CharacterSpawnRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    FormId.Deserialize(aReader);
    BaseId.Deserialize(aReader);
    Position.Deserialize(aReader);
    Rotation.Deserialize(aReader);
    AppearanceBuffer = Serialization::ReadString(aReader);
    InventoryBuffer = Serialization::ReadString(aReader);

    LatestAction = ActionEvent{};
    LatestAction.ApplyDifferential(aReader);
}
