#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

using TiltedPhoques::Vector;

/**
 * @brief Dropped items that the receiving client should place in its world.
 */
struct NotifyDroppedItems final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItems;

    NotifyDroppedItems()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItems& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Items == acRhs.Items; }

    Vector<DroppedItemData> Items{};
};
