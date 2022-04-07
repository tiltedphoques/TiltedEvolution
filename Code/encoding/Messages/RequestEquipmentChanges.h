#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct RequestMagicEquipmentChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestMagicEquipmentChanges;

    RequestMagicEquipmentChanges() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestMagicEquipmentChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestMagicEquipmentChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               ItemId == acRhs.ItemId &&
               EquipSlotId == acRhs.EquipSlotId &&
               Count == acRhs.Count &&
               Unequip == acRhs.Unequip &&
               IsSpell == acRhs.IsSpell &&
               IsShout == acRhs.IsShout &&
               IsAmmo == acRhs.IsAmmo;
    }
    
    uint32_t ServerId{};
    GameId ItemId{};
    GameId EquipSlotId{};
    uint32_t Count{}; // TODO: count can probably be removed
    bool Unequip = false;
    bool IsSpell = false;
    bool IsShout = false;
    bool IsAmmo = false;
};
