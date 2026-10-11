#pragma once

#include <Structs/GameId.h>
#include <Structs/Inventory.h>
#include <Structs/Vector3_NetQuantize.h>

/**
 * @brief Transform of a dropped item reference in the world.
 */
struct DroppedItemTransform
{
    bool operator==(const DroppedItemTransform& acRhs) const noexcept;
    bool operator!=(const DroppedItemTransform& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Vector3_NetQuantize Position{};
    // Euler angles in radians, as stored on the reference.
    glm::vec3 Rotation{};
};

/**
 * @brief Transform update of a dropped item, streamed by the client that simulates its physics.
 */
struct DroppedItemMove
{
    bool operator==(const DroppedItemMove& acRhs) const noexcept;
    bool operator!=(const DroppedItemMove& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t ServerId{};
    DroppedItemTransform Transform{};
    // Set on the last update, once the item has settled.
    bool IsAtRest{};
};

/**
 * @brief A dropped item tracked by the server, sent to clients so they can spawn their own copy.
 */
struct DroppedItemData
{
    bool operator==(const DroppedItemData& acRhs) const noexcept;
    bool operator!=(const DroppedItemData& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t ServerId{};
    Inventory::Entry Item{};
    GameId CellId{};
    GameId WorldSpaceId{};
    DroppedItemTransform Transform{};
    // False while the dropping client is still streaming the item's physics.
    bool IsAtRest{};
};
