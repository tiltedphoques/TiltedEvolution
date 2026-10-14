#pragma once

#include "Message.h"

/**
 * @brief A dropped item was picked up or cleaned up, the receiving client should remove its copy.
 */
struct NotifyDroppedItemRemoved final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItemRemoved;

    NotifyDroppedItemRemoved()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItemRemoved& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId; }

    uint32_t ServerId{};
};
