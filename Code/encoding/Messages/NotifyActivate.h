#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct NotifyActivate final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyActivate;
    NotifyActivate() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActivate& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Id == acRhs.Id && 
               ActivatorId == acRhs.ActivatorId;
    }

    GameId Id;
    uint32_t ActivatorId;
};
