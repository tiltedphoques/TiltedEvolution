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
        return GetOpcode() == acRhs.GetOpcode() &&
               TargetId == acRhs.TargetId &&
               SpellId == acRhs.SpellId && 
               EffectId == acRhs.EffectId &&
               Magnitude == acRhs.Magnitude;
    }

    uint32_t TargetId{};
    GameId SpellId{};
    GameId EffectId{};
    float Magnitude{};
};
