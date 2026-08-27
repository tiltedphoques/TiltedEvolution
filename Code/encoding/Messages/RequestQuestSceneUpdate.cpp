
#include <Messages/RequestQuestSceneUpdate.h>
#include <TiltedCore/Serialization.hpp>

void RequestQuestSceneUpdate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    SceneId.Serialize(aWriter);
    QuestId.Serialize(aWriter);
    aWriter.WriteBits(Stage, 16);
    aWriter.WriteBits(Status, 8);
    aWriter.WriteBits(ClientQuestType, 8);
    aWriter.WriteBits(SceneType, 8);
}

void RequestQuestSceneUpdate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
    SceneId.Deserialize(aReader);
    QuestId.Deserialize(aReader);

    uint64_t tmp;
    aReader.ReadBits(tmp, 16);
    Stage = tmp & 0xFFFF;

    aReader.ReadBits(tmp, 8);
    Status = tmp & 0xFF;

    aReader.ReadBits(tmp, 8);
    ClientQuestType = tmp & 0xFF;

    aReader.ReadBits(tmp, 8);
    SceneType = tmp & 0xFF;
}

