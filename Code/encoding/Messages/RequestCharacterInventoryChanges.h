#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

using TiltedPhoques::Map;

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
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, Inventory> Changes;
};
