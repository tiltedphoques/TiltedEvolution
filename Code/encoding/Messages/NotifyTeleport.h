#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/GameId.h>

struct NotifyTeleport final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTeleport;

    NotifyTeleport() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyTeleport() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyTeleport& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               CellId == acRhs.CellId &&
               Position == acRhs.Position &&
               WorldSpaceId == acRhs.WorldSpaceId;
    }

    GameId CellId{};
    Vector3_NetQuantize Position{};
    GameId WorldSpaceId{};
};
