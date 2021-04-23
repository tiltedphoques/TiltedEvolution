#pragma once

#include "Message.h"

#include <Structs/GameId.h>
#include <Structs/LockData.h>

struct AssignObjectResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kAssignObjectResponse;

    AssignObjectResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignObjectResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && 
            Id == achRhs.Id && 
            CurrentLockData == achRhs.CurrentLockData;
    }

    GameId Id{};
    LockData CurrentLockData{};
};
