#pragma once

#include "Message.h"

#include <Structs/GameId.h>
#include <Structs/Vector3_NetQuantize.h>

struct RequestOwnershipTransfer final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestOwnershipTransfer;

    RequestOwnershipTransfer() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestOwnershipTransfer() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestOwnershipTransfer& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               ServerId == achRhs.ServerId &&
               WorldSpaceId == achRhs.WorldSpaceId &&
               CellId == achRhs.CellId &&
               Position == achRhs.Position;
    }

    uint32_t ServerId{};
    GameId WorldSpaceId{};
    GameId CellId{};
    Vector3_NetQuantize Position{};
};
