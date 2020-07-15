#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Structs/Mods.h>

struct AuthenticationResponse final : ServerMessage
{
    struct Scripts
    {
        Vector<uint8_t> Data{};
    };

    struct ReplicatedObjects
    {
        Vector<uint8_t> Data{};
    };

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
            Scripts.Data == achRhs.Scripts.Data &&
            ReplicatedObjects.Data == achRhs.ReplicatedObjects.Data;
    }

    bool Accepted{ false };
    Mods Mods{};
    Scripts Scripts{};
    ReplicatedObjects ReplicatedObjects{};
};
