#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/GameId.h>

struct TeleportCommandResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kTeleportCommandResponse;

    TeleportCommandResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const TeleportCommandResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               WorldSpaceId == achRhs.WorldSpaceId &&
               CellId == achRhs.CellId &&
               Position == achRhs.Position;
    }

    GameId CellId{};
    Vector3_NetQuantize Position{};
    GameId WorldSpaceId{};
};
