#pragma once

#include "Message.h"

struct NotifySpellCast final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySpellCast;

    NotifySpellCast() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySpellCast& acRhs) const noexcept
    {
        return CasterId == acRhs.CasterId && 
               SpellFormId == acRhs.SpellFormId &&
               CastingSource == acRhs.CastingSource &&
               IsDualCasting == acRhs.IsDualCasting &&
               DesiredTarget == acRhs.DesiredTarget &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t CasterId;
    uint32_t SpellFormId;
    int32_t CastingSource;
    bool IsDualCasting;
    uint32_t DesiredTarget;
};
