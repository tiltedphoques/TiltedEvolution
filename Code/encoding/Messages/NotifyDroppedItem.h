#pragma once

#include "Message.h"

#include <Structs/GameId.h>
#include <Structs/Inventory.h>
#include <Structs/Vector3_NetQuantize.h>

// An item lying in the world after a drop, sent when it is dropped and to players entering its cell.
struct NotifyDroppedItem final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItem;

    NotifyDroppedItem()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItem& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && DropperId == acRhs.DropperId && DropId == acRhs.DropId && Item == acRhs.Item && Position == acRhs.Position &&
               CellId == acRhs.CellId && WorldSpaceId == acRhs.WorldSpaceId;
    }

    uint32_t DropperId{};
    uint32_t DropId{};
    Inventory::Entry Item{};
    Vector3_NetQuantize Position{};
    // Set for interiors, WorldSpaceId for exteriors
    GameId CellId{};
    GameId WorldSpaceId{};
};
