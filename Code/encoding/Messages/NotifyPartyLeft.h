#pragma once

#include "Message.h"

struct NotifyPartyLeft final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPartyLeft;

    NotifyPartyLeft() :
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPartyLeft() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyLeft& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode();
    }

};
