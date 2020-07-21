#pragma once

#include <Messages/ServerMessageFactory.h>

#include <Messages/AuthenticationResponse.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ServerScriptUpdate.h>
#include <Messages/CharacterSpawnRequest.h>

#define EXTRACT_MESSAGE(Name) case k##Name: \
    { \
        auto ptr = TiltedPhoques::MakeUnique<Name>(); \
        ptr->DeserializeRaw(aReader); \
        return TiltedPhoques::CastUnique<ServerMessage>(std::move(ptr)); \
    }

UniquePtr<ServerMessage> ServerMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ServerOpcode) * 8);

    const auto opcode = static_cast<ServerOpcode>(data);
    switch(opcode)
    {
        EXTRACT_MESSAGE(AuthenticationResponse);
        EXTRACT_MESSAGE(AssignCharacterResponse);
        EXTRACT_MESSAGE(ServerReferencesMoveRequest);
        EXTRACT_MESSAGE(ServerScriptUpdate);
        EXTRACT_MESSAGE(CharacterSpawnRequest);
    }

    return UniquePtr<ServerMessage>(nullptr, &TiltedPhoques::Delete<ServerMessage>);
}
