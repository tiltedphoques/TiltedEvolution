#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct NotifyQuestUpdate final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyQuestUpdate;

    NotifyQuestUpdate() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyQuestUpdate& acRhs) const noexcept
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
