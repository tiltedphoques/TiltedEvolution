#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct NotifyNewPackage final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyNewPackage;

    NotifyNewPackage() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyNewPackage& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ActorId == acRhs.ActorId && 
               PackageId == acRhs.PackageId;
    }

    uint32_t ActorId = 0;
    GameId PackageId{};
};
