#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct AddTargetRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kAddTargetRequest;

    AddTargetRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AddTargetRequest& acRhs) const noexcept
    {
        return TargetId == acRhs.TargetId &&
               SpellId == acRhs.SpellId && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t TargetId;
    GameId SpellId{};
};
