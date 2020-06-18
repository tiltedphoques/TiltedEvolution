#pragma once

#include "Message.h"
#include <Buffer.hpp>

struct AuthenticationRequest final : ClientMessage
{
    AuthenticationRequest()
        : ClientMessage(kAuthenticationRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;
    void DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AuthenticationRequest& achRhs) const noexcept
    {
        return Token == achRhs.Token && GetOpcode() == achRhs.GetOpcode();
    }

    String Token;
};
