#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

using TiltedPhoques::Map;

struct RequestInventoryChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestInventoryChanges;

    RequestInventoryChanges() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestInventoryChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, Inventory> Changes;
};
