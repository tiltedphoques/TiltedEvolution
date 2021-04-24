#pragma once

#include "Message.h"
#include <Structs/GameId.h>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct EnterCellRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kEnterCellRequest;

    EnterCellRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~EnterCellRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const EnterCellRequest& acRhs) const noexcept
    {
        return CellId == acRhs.CellId &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId CellId{};
};
