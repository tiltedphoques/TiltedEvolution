#pragma once

#include "Message.h"
#include <Structs/GameId.h>

using TiltedPhoques::Vector;

struct EnterWorldSpaceRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kEnterWorldSpaceRequest;

    EnterWorldSpaceRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~EnterWorldSpaceRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const EnterWorldSpaceRequest& acRhs) const noexcept
    {
        return WorldSpaceId == acRhs.WorldSpaceId &&
            CurrentGridX == acRhs.CurrentGridX &&
            CurrentGridY == acRhs.CurrentGridY &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    GameId WorldSpaceId;
    int32_t CurrentGridX;
    int32_t CurrentGridY;
    Vector<GameId> Cells;
};
