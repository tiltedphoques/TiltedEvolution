#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct NotifyTopic final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTopic;

    NotifyTopic() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyTopic& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               SpeakerID == acRhs.SpeakerID &&
               Type == acRhs.Type &&
               TopicID1 == acRhs.TopicID1 &&
               TopicID2 == acRhs.TopicID2;
    }
    
    uint32_t SpeakerID;
    uint32_t Type;
    GameId TopicID1;
    GameId TopicID2;
};
