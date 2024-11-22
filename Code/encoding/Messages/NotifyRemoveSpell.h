#pragma once
#include "Message.h"
#include <Structs/GameId.h>

struct NotifyRemoveSpell final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyRemoveSpell;

    NotifyRemoveSpell() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;

    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRemoveSpell& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && TargetId == acRhs.TargetId && SpellId == acRhs.SpellId;
    }

    uint32_t TargetId{};
    GameId SpellId{};
};
