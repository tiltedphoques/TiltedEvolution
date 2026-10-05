#pragma once

#include "Message.h"
#include <Structs/GameId.h>

// Sent when the local player enters or leaves power armor. A null furniture base means out of power armor.
struct PowerArmorRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPowerArmorRequest;

    PowerArmorRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PowerArmorRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id && FurnitureId == acRhs.FurnitureId && FurnitureBaseId == acRhs.FurnitureBaseId && GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id{};
    GameId FurnitureId{};
    GameId FurnitureBaseId{};
};
