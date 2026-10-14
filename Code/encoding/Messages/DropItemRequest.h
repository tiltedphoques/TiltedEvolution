#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

/**
 * @brief Sent when a local actor drops an item into the world.
 */
struct DropItemRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kDropItemRequest;

    DropItemRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DropItemRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && RefId == acRhs.RefId && Data == acRhs.Data; }

    // Local form id of the reference the drop created, the server echoes it back with the server id.
    uint32_t RefId{};
    DroppedItemData Data{};
};
