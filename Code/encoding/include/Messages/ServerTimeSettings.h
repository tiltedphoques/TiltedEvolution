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
        return ServerTick == achRhs.ServerTick && TimeScale == achRhs.TimeScale &&
               GetOpcode() == achRhs.GetOpcode();
    }

    uint64_t ServerTick;
    float Time;
    float TimeScale;
};
