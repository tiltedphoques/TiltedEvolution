#pragma once

struct PartyJoinedEvent
{
    PartyJoinedEvent() = delete;
    PartyJoinedEvent(bool aIsLeader)
        : IsLeader(aIsLeader)
    {}

    bool IsLeader{};
};
