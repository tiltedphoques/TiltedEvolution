#pragma once

#include "Message.h"
#include "Structs/GameId.h"
#include "Structs/Vector3_NetQuantize.h"

#include <Buffer.hpp>

struct NotifyCharacterTravel final : ServerMessage
{
    NotifyCharacterTravel() : ServerMessage(kNotifyCharacterTravel)
    {
    }

    virtual ~NotifyCharacterTravel() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyCharacterTravel& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId && 
            CellId == achRhs.CellId &&
            Position == achRhs.Position &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
    GameId CellId;
    Vector3_NetQuantize Position;
};
