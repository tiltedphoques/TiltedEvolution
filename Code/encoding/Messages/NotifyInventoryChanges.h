#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/Inventory.h>

using TiltedPhoques::Map;

struct NotifyInventoryChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyInventoryChanges;

    NotifyInventoryChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, Inventory> Changes{};
};
