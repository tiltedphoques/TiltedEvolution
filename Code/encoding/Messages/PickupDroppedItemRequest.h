#pragma once

#include "Message.h"

/**
 * @brief Sent when a local actor picks up an item that is tracked as a dropped item by the server.
 */
struct PickupDroppedItemRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPickupDroppedItemRequest;

    PickupDroppedItemRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PickupDroppedItemRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId; }

    uint32_t ServerId{};
};
