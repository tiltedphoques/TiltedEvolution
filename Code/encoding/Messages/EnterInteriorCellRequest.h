#pragma once

#include "Message.h"
#include <Structs/GameId.h>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct EnterInteriorCellRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kEnterInteriorCellRequest;

    EnterInteriorCellRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~EnterInteriorCellRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const EnterInteriorCellRequest& acRhs) const noexcept
    {
        return CellId == acRhs.CellId &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId CellId{};
};
