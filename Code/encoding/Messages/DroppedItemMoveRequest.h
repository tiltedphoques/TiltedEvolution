#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

/**
 * @brief Streams the transform of a dropped item while its physics runs on the dropping client.
 */
struct DroppedItemMoveRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kDroppedItemMoveRequest;

    DroppedItemMoveRequest()
        : ClientMessage(Opcode)
    {
    }

    virtual ~DroppedItemMoveRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DroppedItemMoveRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Move == acRhs.Move; }

    DroppedItemMove Move{};
};
