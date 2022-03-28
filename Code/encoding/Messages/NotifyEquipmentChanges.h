#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/Equipment.h>
#include <Structs/Inventory.h>

struct NotifyEquipmentChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyEquipmentChanges;

    NotifyEquipmentChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyEquipmentChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               EquippedItem == acRhs.EquippedItem &&
               IsLeft == acRhs.IsLeft &&
               IsSpell == acRhs.IsSpell &&
               IsShout == acRhs.IsShout;
    }
    
    uint32_t ServerId{};
    Inventory::Entry EquippedItem{};
    bool IsLeft = false;
    bool IsSpell = false;
    bool IsShout = false;
};
