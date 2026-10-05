#pragma once

#include "Message.h"
#include <Structs/GameId.h>

// A player entered or left power armor. A null furniture base means out of power armor.
struct NotifyPowerArmor final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPowerArmor;

    NotifyPowerArmor()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPowerArmor& acRhs) const noexcept
    {
        return Id == acRhs.Id && FurnitureId == acRhs.FurnitureId && FurnitureBaseId == acRhs.FurnitureBaseId && GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id{};
    GameId FurnitureId{};
    GameId FurnitureBaseId{};
};
