#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/Inventory.h>

struct NotifyCharacterInventoryChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyCharacterInventoryChanges;

    NotifyCharacterInventoryChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyCharacterInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    TiltedPhoques::Map<uint32_t, Inventory> Changes{};
};
