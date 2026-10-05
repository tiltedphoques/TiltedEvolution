#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

/**
 * @brief Relays a dropped item's transform from the client that simulates its physics.
 */
struct NotifyDroppedItemMove final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItemMove;

    NotifyDroppedItemMove()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItemMove& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Move == acRhs.Move; }

    DroppedItemMove Move{};
};
