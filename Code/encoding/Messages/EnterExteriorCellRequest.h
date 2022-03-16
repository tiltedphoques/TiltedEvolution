#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>

using TiltedPhoques::String;

struct EnterExteriorCellRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kEnterExteriorCellRequest;

    EnterExteriorCellRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~EnterExteriorCellRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const EnterExteriorCellRequest& acRhs) const noexcept
    {
        return WorldSpaceId == acRhs.WorldSpaceId &&
            CellId == acRhs.CellId &&
            CurrentCoords == acRhs.CurrentCoords &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId WorldSpaceId{};
    GameId CellId{};
    GridCellCoords CurrentCoords{};
};
