#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/Scripts.h>
#include <Structs/FullObjects.h>

struct AuthenticationResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kAuthenticationResponse;

    AuthenticationResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AuthenticationResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && 
            Accepted == achRhs.Accepted && 
            UserMods == achRhs.UserMods && 
            ServerScripts == achRhs.ServerScripts &&
            ReplicatedObjects == achRhs.ReplicatedObjects;
    }

    bool Accepted{ false };
    Mods UserMods{};
    Scripts ServerScripts{};
    FullObjects ReplicatedObjects{};
};
