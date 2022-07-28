#pragma once

#include "Message.h"

#include <Structs/Inventory.h>

struct NotifyObjectInventoryChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyObjectInventoryChanges;

    NotifyObjectInventoryChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyObjectInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    TiltedPhoques::Map<GameId, Inventory> Changes{};
};
