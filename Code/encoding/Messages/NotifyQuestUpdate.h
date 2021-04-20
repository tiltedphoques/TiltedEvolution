#pragma once

#include "Message.h"
#include <Structs/GameId.h>

using TiltedPhoques::Map;

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
        return Id == acRhs.Id && Status == acRhs.Status && Stage == acRhs.Stage && GetOpcode() == acRhs.GetOpcode();

    }

    enum StatusCode : uint8_t
    {
        StageUpdate,
        Started,
        Stopped
    };

    GameId Id;
    uint8_t Status; 
    uint16_t Stage;
};
