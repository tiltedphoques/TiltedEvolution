#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct LockChangeRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kLockChangeRequest;

    LockChangeRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const LockChangeRequest& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               CellId == acRhs.CellId &&
               IsLocked == acRhs.IsLocked &&
               LockLevel == acRhs.LockLevel &&
               GetOpcode() == acRhs.GetOpcode();
    }

    GameId Id;
    GameId CellId;
    uint8_t IsLocked;
    uint8_t LockLevel;
};
