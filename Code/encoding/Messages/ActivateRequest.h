#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct ActivateRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kActivateRequest;

    ActivateRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ActivateRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               ActivatorId == acRhs.ActivatorId &&
               GetOpcode() == acRhs.GetOpcode();
    }

    GameId Id;
    GameId CellId;
    uint32_t ActivatorId;
};
