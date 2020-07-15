#pragma once

#include <Messages/ServerMessageFactory.h>

#include <Messages/AuthenticationResponse.h>

#define EXTRACT_MESSAGE(Name) case k##Name: \
    { \
        auto ptr = TiltedPhoques::MakeUnique<Name>(); \
        ptr->DeserializeRaw(aReader); \
        return TiltedPhoques::CastUnique<ServerMessage>(std::move(ptr)); \
    }

UniquePtr<ServerMessage> ServerMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ClientOpcode) * 8);

    const auto opcode = static_cast<ClientOpcode>(data);
    switch(opcode)
    {
        EXTRACT_MESSAGE(AuthenticationResponse);
    }

    return UniquePtr<ServerMessage>(nullptr, &TiltedPhoques::Delete<ServerMessage>);
}
