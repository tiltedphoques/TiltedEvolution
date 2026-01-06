
#include <Messages/NotifyQuestSceneUpdate.h>
#include <TiltedCore/Serialization.hpp>

void NotifyQuestSceneUpdate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    SceneId.Serialize(aWriter);
    QuestId.Serialize(aWriter);
    aWriter.WriteBits(SceneType, 8);
}

void NotifyQuestSceneUpdate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);
    SceneId.Deserialize(aReader);
    QuestId.Deserialize(aReader);

    uint64_t tmp;
    aReader.ReadBits(tmp, 8);
    SceneType = tmp & 0xFF;
}
