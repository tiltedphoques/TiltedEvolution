#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

/**
 * @brief Sent when the local player drops an item, so the server can track it and tell the other clients.
 */
struct DropItemRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kDropItemRequest;

    DropItemRequest()
        : ClientMessage(Opcode)
    {
    }

    virtual ~DropItemRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DropItemRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && LocalId == acRhs.LocalId && Item == acRhs.Item && CellId == acRhs.CellId && WorldSpaceId == acRhs.WorldSpaceId && Transform == acRhs.Transform;
    }

    // Form id of the dropping client's reference, echoed back in DropItemResponse.
    uint32_t LocalId{};
    Inventory::Entry Item{};
    GameId CellId{};
    GameId WorldSpaceId{};
    DroppedItemTransform Transform{};
};
