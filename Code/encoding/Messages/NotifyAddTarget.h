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
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t TargetId;
    GameId SpellId{};
};
