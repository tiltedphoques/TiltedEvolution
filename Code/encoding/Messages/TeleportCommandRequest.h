#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct TeleportCommandRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTeleportCommandRequest;

    TeleportCommandRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const TeleportCommandRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               TargetPlayer == acRhs.TargetPlayer;
    }

    TiltedPhoques::String TargetPlayer;
};

