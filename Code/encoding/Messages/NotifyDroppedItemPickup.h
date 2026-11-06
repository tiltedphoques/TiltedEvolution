#pragma once

#include "Message.h"

// A dropped item was picked up, other clients delete their copy of it.
struct NotifyDroppedItemPickup final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItemPickup;

    NotifyDroppedItemPickup()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItemPickup& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && DropperId == acRhs.DropperId && DropId == acRhs.DropId && Result == acRhs.Result; }

    uint32_t DropperId{};
    uint32_t DropId{};
    enum : uint8_t
    {
        // Someone else picked it up, delete the local copy
        kTaken,
        // Sent to the picker
        kConfirmed,
        // Sent to the picker when someone else got it first
        kDenied,
    };
    uint8_t Result = kTaken;
};
