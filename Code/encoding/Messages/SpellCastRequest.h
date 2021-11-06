#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct SpellCastRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kSpellCastRequest;

    SpellCastRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SpellCastRequest& acRhs) const noexcept
    {
        return CasterId == acRhs.CasterId &&
               SpellFormId == acRhs.SpellFormId &&
               CastingSource == acRhs.CastingSource &&
               IsDualCasting == acRhs.IsDualCasting &&
               DesiredTarget == acRhs.DesiredTarget &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t CasterId;
    GameId SpellFormId{};
    int32_t CastingSource;
    bool IsDualCasting;
    uint32_t DesiredTarget;
};
