#pragma once

#include "Message.h"

struct MountRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kMountRequest;

    MountRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const MountRequest& acRhs) const noexcept
    {
        return RiderId == acRhs.RiderId &&
               MountId == acRhs.MountId &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t RiderId;
    uint32_t MountId;
};
