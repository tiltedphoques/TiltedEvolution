#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct RequestWeatherChange final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestWeatherChange;

    RequestWeatherChange() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestWeatherChange& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && 
               Id == acRhs.Id;
    }

    GameId Id;
};
