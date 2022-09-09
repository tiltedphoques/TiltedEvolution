#pragma once

#include "Message.h"

#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

struct NotifyPlayerJoined final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerJoined;

    NotifyPlayerJoined() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifyPlayerJoined() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerJoined& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId &&
               Username == acRhs.Username &&
               WorldSpaceId == acRhs.WorldSpaceId &&
               CellId == acRhs.CellId &&
               Level == acRhs.Level &&
               CenterCoords == acRhs.CenterCoords;
    }

    uint32_t PlayerId{};
    String Username{};
    GameId WorldSpaceId{};
    GameId CellId{};
    uint16_t Level{};
    GridCellCoords CenterCoords{};
};
