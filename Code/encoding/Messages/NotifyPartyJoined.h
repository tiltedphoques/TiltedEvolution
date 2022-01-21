#pragma once

#include "Message.h"

using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct NotifyPartyJoined final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPartyJoined;

    NotifyPartyJoined() :
        ServerMessage(Opcode),
        IsLeader(false)
    {
    }

    virtual ~NotifyPartyJoined() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyJoined& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
                PlayerIds == acRhs.PlayerIds &&
                IsLeader == acRhs.IsLeader &&
                LeaderPlayerId == acRhs.LeaderPlayerId;
    }

    Vector<uint32_t> PlayerIds{};
    bool IsLeader;
    uint32_t LeaderPlayerId;
};
