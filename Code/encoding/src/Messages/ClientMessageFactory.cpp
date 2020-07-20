#pragma once

#include <Messages/ClientMessageFactory.h>

#include <Messages/AuthenticationRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/RemoveCharacterRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/EnterCellRequest.h>

#include <iostream>

#define EXTRACT_MESSAGE(Name) case k##Name: \
    { \
        auto ptr = TiltedPhoques::MakeUnique<Name>(); \
        ptr->DeserializeRaw(aReader); \
        std::cout << "Packet received type " << #Name << std::endl; \
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
        EXTRACT_MESSAGE(AssignCharacterRequest);
        EXTRACT_MESSAGE(CancelAssignmentRequest);
        EXTRACT_MESSAGE(RemoveCharacterRequest);
        EXTRACT_MESSAGE(ClientReferencesMoveRequest);
        EXTRACT_MESSAGE(EnterCellRequest);
    }

    return UniquePtr<ClientMessage>(nullptr, &TiltedPhoques::Delete<ClientMessage>);
}
