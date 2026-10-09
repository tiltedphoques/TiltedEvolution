#pragma once

#include "Message.h"
#include <Structs/SettlementObject.h>

struct NotifySettlement final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySettlement;
    NotifySettlement() : ServerMessage(Opcode) {}
    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;
    bool operator==(const NotifySettlement& acOther) const noexcept
    {
        return SessionId == acOther.SessionId && LeaderPlayerId == acOther.LeaderPlayerId && SenderId == acOther.SenderId && RequestId == acOther.RequestId &&
               Accepted == acOther.Accepted && Data == acOther.Data;
    }

    uint64_t SessionId{};
    uint32_t LeaderPlayerId{};
    uint32_t SenderId{};
    uint32_t RequestId{};
    bool Accepted{};
    SettlementObject Data{};
};
