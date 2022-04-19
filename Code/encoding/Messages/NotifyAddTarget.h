#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct NotifyAddTarget final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyAddTarget;

    NotifyAddTarget() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyAddTarget& acRhs) const noexcept
    {
        return TargetId == acRhs.TargetId && 
               SpellId == acRhs.SpellId &&
               EffectId == acRhs.EffectId &&
               Magnitude == acRhs.Magnitude &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t TargetId{};
    GameId SpellId{};
    GameId EffectId{};
    float Magnitude{};
};
