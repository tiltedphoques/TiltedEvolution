#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct NotifyPlayerCellChanged final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerCellChanged;

    NotifyPlayerCellChanged() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPlayerCellChanged() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerCellChanged& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId &&
               WorldSpaceId == acRhs.WorldSpaceId &&
               CellId == acRhs.CellId;
    }

    uint32_t PlayerId;
    GameId WorldSpaceId;
    GameId CellId;
};
