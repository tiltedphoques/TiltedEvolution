#pragma once

#include "Message.h"

/**
 * @brief Sent when a local actor picks up a server-tracked dropped item.
 */
struct PickUpDroppedItemRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPickUpDroppedItemRequest;

    PickUpDroppedItemRequest()
        : ClientMessage(Opcode)
    {
    }

    virtual ~PickUpDroppedItemRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PickUpDroppedItemRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId; }

    uint32_t ServerId{};
};
