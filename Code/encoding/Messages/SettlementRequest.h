#pragma once

#include "Message.h"
#include <Structs/SettlementObject.h>

struct SettlementRequest final : ClientMessage
{
    enum class Action : uint8_t { Snapshot, Update };
    static constexpr ClientOpcode Opcode = kSettlementRequest;

    SettlementRequest() : ClientMessage(Opcode) {}
    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;
    bool operator==(const SettlementRequest& acOther) const noexcept
    {
        return Operation == acOther.Operation && SessionId == acOther.SessionId && RequestId == acOther.RequestId && Data == acOther.Data;
    }

    Action Operation{Action::Snapshot};
    uint64_t SessionId{};
    uint32_t RequestId{};
    SettlementObject Data{};
};
