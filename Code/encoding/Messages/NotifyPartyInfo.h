#pragma once

#include "Message.h"

using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct NotifyPartyInfo final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPartyInfo;

    NotifyPartyInfo()
        : ServerMessage(Opcode)
        , IsLeader(false)
        , AllowAutoJoin{true}
        , ServerAutoJoin{}
        , PartyCount{}
    {
    }

    virtual ~NotifyPartyInfo() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyInfo& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && PlayerIds == acRhs.PlayerIds && LeaderPlayerId == acRhs.LeaderPlayerId && AllowAutoJoin == acRhs.AllowAutoJoin && ServerAutoJoin == acRhs.ServerAutoJoin && PartyCount == acRhs.PartyCount; }

    Vector<uint32_t> PlayerIds{};
    bool IsLeader;
    uint32_t LeaderPlayerId;
    bool AllowAutoJoin{true};
    bool ServerAutoJoin{};
    uint32_t PartyCount{};
};
