#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

struct NotifyInventorySnapshot final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyInventorySnapshot;

    NotifyInventorySnapshot()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyInventorySnapshot& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId && OwnershipEpoch == acRhs.OwnershipEpoch && CurrentInventory == acRhs.CurrentInventory;
    }

    uint32_t ServerId{};
    uint32_t OwnershipEpoch{};
    Inventory CurrentInventory{};
};
