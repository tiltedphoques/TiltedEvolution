#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct TopicRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTopicRequest;

    TopicRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~TopicRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const TopicRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && 
               SpeakerID == acRhs.SpeakerID &&
               Type == acRhs.Type &&
               TopicID1 == acRhs.TopicID1 &&
               TopicID2 == acRhs.TopicID2;
    }
    
    uint32_t SpeakerID{};
    uint32_t Type{};
    GameId TopicID1{};
    GameId TopicID2{};
};
