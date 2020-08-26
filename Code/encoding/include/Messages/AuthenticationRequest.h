#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Structs/Mods.h>

struct AuthenticationRequest final : ClientMessage
{
    AuthenticationRequest()
        : ClientMessage(kAuthenticationRequest)
    {
    }

    virtual ~AuthenticationRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AuthenticationRequest& achRhs) const noexcept
    {
        return Token == achRhs.Token &&
            Mods == achRhs.Mods &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint64_t DiscordId;
    String Token;
    Mods Mods;
};
