#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/Scripts.h>
#include <Structs/FullObjects.h>

struct AuthenticationResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kAuthenticationResponse;

    enum class ResponseType : uint8_t
    {
        kAccepted,
        kWrongVersion,
        kMissingMods,
    };

    AuthenticationResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AuthenticationResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && Type == achRhs.Type && UserMods == achRhs.UserMods;
    }

    ResponseType Type;
    String Version;
    Mods UserMods{};
};
