#pragma once

#include "Message.h"

// A dropped item was picked up, other clients delete their copy of it.
struct RequestDroppedItemPickup final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestDroppedItemPickup;

    RequestDroppedItemPickup()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestDroppedItemPickup& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && DropperId == acRhs.DropperId && DropId == acRhs.DropId; }

    uint32_t DropperId{};
    uint32_t DropId{};
};
