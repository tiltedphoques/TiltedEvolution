#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct NotifyPlayerPartyLeft final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerPartyLeft;

    NotifyPlayerPartyLeft() :
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPlayerPartyLeft() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerPartyLeft& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t LeavingPlayerId;
};
