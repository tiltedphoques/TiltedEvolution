#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct NotifyLockChange final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyLockChange;
    NotifyLockChange() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyLockChange& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               IsLocked == acRhs.IsLocked &&
               LockLevel == acRhs.LockLevel &&
               GetOpcode() == acRhs.GetOpcode();
    }

    GameId Id;
    bool IsLocked;
    uint8_t LockLevel;
};
