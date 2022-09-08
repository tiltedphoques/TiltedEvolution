#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct RequestCurrentWeather final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestCurrentWeather;

    RequestCurrentWeather() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestCurrentWeather& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }
};
