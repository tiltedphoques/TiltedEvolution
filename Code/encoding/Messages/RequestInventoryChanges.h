#pragma once

#include "Message.h"
#include <Structs/Inventory.h>
#include <Structs/Vector3_NetQuantize.h>

struct RequestInventoryChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestInventoryChanges;

    RequestInventoryChanges()
        : ClientMessage(Opcode)
    {
    }

    virtual ~RequestInventoryChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestInventoryChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId && OwnershipEpoch == acRhs.OwnershipEpoch && Item == acRhs.Item && Drop == acRhs.Drop && DropId == acRhs.DropId && DropPosition == acRhs.DropPosition && UpdateClients == acRhs.UpdateClients;
    }

    uint32_t ServerId{};
    uint32_t OwnershipEpoch{};
    Inventory::Entry Item{};
    bool Drop = false;
    // Identifies the dropped reference across clients, together with ServerId
    uint32_t DropId{};
    // Where the dropped reference was placed, only sent with a DropId
    Vector3_NetQuantize DropPosition{};
    bool UpdateClients = true;
};
