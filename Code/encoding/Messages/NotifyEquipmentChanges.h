#pragma once

#include "Message.h"

#include <Structs/GameId.h>

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
               ItemId == acRhs.ItemId &&
               EquipSlotId == acRhs.EquipSlotId &&
               Count == acRhs.Count &&
               Unequip == acRhs.Unequip &&
               IsSpell == acRhs.IsSpell &&
               IsShout == acRhs.IsShout;
    }
    
    uint32_t ServerId{};
    GameId ItemId{};
    GameId EquipSlotId{};
    uint32_t Count{};
    bool Unequip = false;
    bool IsSpell = false;
    bool IsShout = false;
};
