#pragma once

#include "Message.h"
#include <Structs/ActorData.h>

struct NotifyOwnershipTransfer final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyOwnershipTransfer;
    NotifyOwnershipTransfer()
        : ServerMessage(Opcode)
    {
    }

    virtual ~NotifyOwnershipTransfer() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyOwnershipTransfer& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId && OwnerPlayerId == achRhs.OwnerPlayerId && OwnershipEpoch == achRhs.OwnershipEpoch && CurrentActorData == achRhs.CurrentActorData && GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId{};
    uint32_t OwnerPlayerId{};
    uint32_t OwnershipEpoch{};
    ActorData CurrentActorData{};
};
