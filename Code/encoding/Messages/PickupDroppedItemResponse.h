#pragma once

#include "Message.h"

/**
 * @brief Tells the client whether its pickup of a dropped item went through.
 */
struct PickupDroppedItemResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kPickupDroppedItemResponse;

    PickupDroppedItemResponse()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PickupDroppedItemResponse& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId && Accepted == acRhs.Accepted; }

    uint32_t ServerId{};
    // False if someone else got to the item first.
    bool Accepted{};
};
