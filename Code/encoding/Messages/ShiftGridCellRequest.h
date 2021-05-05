#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

using TiltedPhoques::Vector;

struct ShiftGridCellRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kShiftGridCellRequest;

    ShiftGridCellRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~ShiftGridCellRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ShiftGridCellRequest& acRhs) const noexcept
    {
        return WorldSpaceId == acRhs.WorldSpaceId &&
            CenterCoords == acRhs.CenterCoords &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId WorldSpaceId;
    GameId PlayerCell;
    GridCellCoords CenterCoords;
    GridCellCoords PlayerCoords;
    Vector<GameId> Cells;
};
