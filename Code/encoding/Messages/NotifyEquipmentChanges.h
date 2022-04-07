#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/GameId.h>

struct NotifyMagicEquipmentChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyMagicEquipmentChanges;

    NotifyMagicEquipmentChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyMagicEquipmentChanges& acRhs) const noexcept
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
    uint32_t Count{}; // TODO: count can probably be removed
    bool Unequip = false;
    bool IsSpell = false;
    bool IsShout = false;
};
