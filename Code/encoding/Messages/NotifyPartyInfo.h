#pragma once

#include "Message.h"

using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct NotifyPartyInfo final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPartyInfo;

    NotifyPartyInfo() :
        ServerMessage(Opcode), IsLeader(false)
    {
    }

    virtual ~NotifyPartyInfo() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyInfo& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
            PlayerIds == acRhs.PlayerIds &&
            LeaderPlayerId == acRhs.LeaderPlayerId;
    }

    Vector<uint32_t> PlayerIds{};
    bool IsLeader;
    uint32_t LeaderPlayerId;
};
