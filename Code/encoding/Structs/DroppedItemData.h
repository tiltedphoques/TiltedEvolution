#pragma once

#include <Structs/GameId.h>
#include <Structs/Inventory.h>

/**
 * @brief An item a player dropped into the world, as tracked by the server.
 */
struct DroppedItemData
{
    DroppedItemData() = default;
    ~DroppedItemData() = default;

    bool operator==(const DroppedItemData& acRhs) const noexcept;
    bool operator!=(const DroppedItemData& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t ServerId{};
    // Count is always positive, it is the size of the dropped stack.
    Inventory::Entry Item{};
    GameId CellId{};
    GameId WorldSpaceId{};
    float PositionX{};
    float PositionY{};
    float PositionZ{};
    float RotationX{};
    float RotationY{};
    float RotationZ{};
};
