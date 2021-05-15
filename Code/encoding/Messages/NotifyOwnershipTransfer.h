#pragma once

#include "Message.h"

struct NotifyOwnershipTransfer final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyOwnershipTransfer;
    NotifyOwnershipTransfer() : ServerMessage(Opcode)
    {
    }

    virtual ~NotifyOwnershipTransfer() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyOwnershipTransfer& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId && GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
};
