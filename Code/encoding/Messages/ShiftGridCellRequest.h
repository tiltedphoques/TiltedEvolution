#pragma once

#include "Message.h"
#include <Structs/GameId.h>

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
            CurrentGridX == acRhs.CurrentGridX &&
            CurrentGridY == acRhs.CurrentGridY &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId WorldSpaceId;
    GameId PlayerCell;
    int32_t CurrentGridX;
    int32_t CurrentGridY;
    Vector<GameId> Cells;
};
