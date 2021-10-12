#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/Inventory.h>

using TiltedPhoques::Map;

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
    
    Map<GameId, Inventory> Changes{};
};
