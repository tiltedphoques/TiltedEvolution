#pragma once

#include "Message.h"

struct NotifyPlayerLeft final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerLeft;

    NotifyPlayerLeft() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPlayerLeft() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerLeft& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               Username == acRhs.Username;
    }

    uint64_t ServerId;
    String Username;
};
