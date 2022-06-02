#pragma once

#include "Message.h"

#include <Structs/Inventory.h>

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
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               Item == acRhs.Item &&
               Drop == acRhs.Drop;
    }
    
    uint32_t ServerId{};
    Inventory::Entry Item{};
    bool Drop = false;
};
