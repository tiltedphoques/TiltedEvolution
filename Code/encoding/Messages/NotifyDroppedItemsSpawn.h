#pragma once

#include "Message.h"

#include <Structs/DroppedItemData.h>

using TiltedPhoques::Vector;

/**
 * @brief Tells a client to spawn its copy of dropped items that came into its range.
 */
struct NotifyDroppedItemsSpawn final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItemsSpawn;

    NotifyDroppedItemsSpawn()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItemsSpawn& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Items == acRhs.Items; }

    Vector<DroppedItemData> Items{};
};
