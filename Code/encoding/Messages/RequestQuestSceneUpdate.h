#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct RequestQuestSceneUpdate final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestQuestSceneUpdate;

    RequestQuestSceneUpdate()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestQuestSceneUpdate& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && SceneId == acRhs.SceneId && QuestId == acRhs.QuestId && Stage == acRhs.Stage && Status == acRhs.Status && ClientQuestType == acRhs.ClientQuestType && SceneType == acRhs.SceneType;
    }

    GameId SceneId;
    GameId QuestId;
    uint16_t Stage;
    uint8_t Status;
    uint8_t ClientQuestType;
    uint8_t SceneType; // BEGIN (0) or END (1)
};
