#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>
#include <Structs/GameId.h>

struct RequestQuestUpdate final : ClientMessage
{
    RequestQuestUpdate() : ClientMessage(kRequestQuestUpdate)
    {
    }

    virtual ~RequestQuestUpdate() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestQuestUpdate& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
            Stage == acRhs.Stage && 
            Status == acRhs.Stage &&
            GetOpcode() == acRhs.GetOpcode();
    }

    enum StatusCode : uint8_t
    {
        StageUpdate,
        Stopped,
        Started
    };

    GameId Id;
    uint16_t Stage;
    uint8_t Status;
    //bool Active;
};
