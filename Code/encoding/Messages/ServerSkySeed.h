#pragma once

#include "Message.h"
#include <Structs/Objects.h>

struct ServerSkySeed final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kServerSkySeed;

    ServerSkySeed() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ServerSkySeed& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && Seed == achRhs.Seed;
    }

    uint32_t Seed;
};
