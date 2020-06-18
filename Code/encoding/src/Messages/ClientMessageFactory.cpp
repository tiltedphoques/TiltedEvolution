#pragma once

#include <Messages/ClientMessageFactory.h>

#include <Messages/AuthenticationRequest.h>

#define EXTRACT_MESSAGE(Name) case k##Name: \
    { \
        auto ptr = TiltedPhoques::MakeUnique<Name>(); \
        ptr->DeserializeRaw(aReader); \
        return TiltedPhoques::CastUnique<ClientMessage>(std::move(ptr)); \
    }

UniquePtr<ClientMessage> ClientMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ClientOpcode) * 8);

    const auto opcode = static_cast<ClientOpcode>(data);
    switch(opcode)
    {
        EXTRACT_MESSAGE(AuthenticationRequest);
    }

    return UniquePtr<ClientMessage>(nullptr, &TiltedPhoques::Delete<ClientMessage>);
}
