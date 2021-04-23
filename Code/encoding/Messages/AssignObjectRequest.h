#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/LockData.h>

struct AssignObjectRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kAssignObjectRequest;

    AssignObjectRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~AssignObjectRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignObjectRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id &&
            CellId == acRhs.CellId &&
            CurrentLockdata == acRhs.CurrentLockdata &&
            GetOpcode() == acRhs.GetOpcode();
    }

    GameId Id{};
    GameId CellId{};
    LockData CurrentLockdata{};
};
