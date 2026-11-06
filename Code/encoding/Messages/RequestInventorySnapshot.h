#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

struct RequestInventorySnapshot final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestInventorySnapshot;

    RequestInventorySnapshot()
        : ClientMessage(Opcode)
    {
    }

    virtual ~RequestInventorySnapshot() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestInventorySnapshot& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId && OwnershipEpoch == acRhs.OwnershipEpoch && CurrentInventory == acRhs.CurrentInventory;
    }

    uint32_t ServerId{};
    uint32_t OwnershipEpoch{};
    Inventory CurrentInventory{};
};
