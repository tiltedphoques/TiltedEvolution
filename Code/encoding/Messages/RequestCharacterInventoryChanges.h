#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

struct RequestCharacterInventoryChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestCharacterInventoryChanges;

    RequestCharacterInventoryChanges() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestCharacterInventoryChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestCharacterInventoryChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ActorId == acRhs.ActorId &&
               Item == acRhs.Item;
    }
    
    uint32_t ActorId{};
    Inventory::Entry Item{};
};
