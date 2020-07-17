#include <Messages/AssignCharacterRequest.h>

void AssignCharacterRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Cookie);
    ReferenceId.Serialize(aWriter);
    FormId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
    Rotation.Serialize(aWriter);
    Serialization::WriteString(aWriter, AppearanceBuffer);
    LatestAction.GenerateDifferential(ActionEvent{}, aWriter);
}

void AssignCharacterRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Cookie = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ReferenceId.Deserialize(aReader);
    FormId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
    Rotation.Deserialize(aReader);
    AppearanceBuffer = Serialization::ReadString(aReader);

    LatestAction = ActionEvent{};
    LatestAction.ApplyDifferential(aReader);
}
