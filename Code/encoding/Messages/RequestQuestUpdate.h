#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct RequestQuestUpdate final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestQuestUpdate;

    RequestQuestUpdate() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestQuestUpdate& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Id == acRhs.Id && 
               Stage == acRhs.Stage && 
               Status == acRhs.Stage;
    }

    enum StatusCode : uint8_t
    {
        StageUpdate,
        Started,
        Stopped
    };

    GameId Id;
    uint16_t Stage;
    uint8_t Status;
};
