#pragma once

#include "Message.h"

struct NotifyMount final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyMount;

    NotifyMount() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyMount& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               RiderId == acRhs.RiderId &&
               MountId == acRhs.MountId;
    }

    uint32_t RiderId;
    uint32_t MountId;
};
