#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Structs/Objects.h>

struct ServerTimeSettings final : ServerMessage
{
    ServerTimeSettings() : ServerMessage(kServerTimeSettings)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer &aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader &aReader) noexcept override;

    bool operator==(const ServerTimeSettings &achRhs) const noexcept
    {
        return Time == achRhs.Time && TimeScale == achRhs.TimeScale &&
               GetOpcode() == achRhs.GetOpcode();
    }

    float Time;
    float TimeScale;
};
