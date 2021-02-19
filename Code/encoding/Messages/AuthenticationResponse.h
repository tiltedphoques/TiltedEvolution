#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/Scripts.h>
#include <Structs/FullObjects.h>

struct AuthenticationResponse final : ServerMessage
{
    AuthenticationResponse()
        : ServerMessage(kAuthenticationResponse)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AuthenticationResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && 
            Accepted == achRhs.Accepted && 
            Mods == achRhs.Mods && 
            Scripts == achRhs.Scripts &&
            ReplicatedObjects == achRhs.ReplicatedObjects;
    }

    bool Accepted{ false };
    Mods Mods{};
    Scripts Scripts{};
    FullObjects ReplicatedObjects{};
};
