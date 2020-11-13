#pragma once

#include <Messages/ClientMessageFactory.h>

#include <Messages/AuthenticationRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/RemoveCharacterRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/EnterCellRequest.h>
#include <Messages/ClientRpcCalls.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/RequestQuestUpdate.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/PartyAcceptInviteRequest.h>
#include <Messages/PartyLeaveRequest.h>
#include <Messages/CharacterTravelRequest.h>
#include <Messages/RequestActorValueChanges.h>

#include <iostream>

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
        EXTRACT_MESSAGE(AssignCharacterRequest);
        EXTRACT_MESSAGE(CancelAssignmentRequest);
        EXTRACT_MESSAGE(RemoveCharacterRequest);
        EXTRACT_MESSAGE(ClientReferencesMoveRequest);
        EXTRACT_MESSAGE(EnterCellRequest);
        EXTRACT_MESSAGE(ClientRpcCalls);
        EXTRACT_MESSAGE(RequestInventoryChanges);
        EXTRACT_MESSAGE(RequestFactionsChanges);
        EXTRACT_MESSAGE(RequestQuestUpdate);
        EXTRACT_MESSAGE(PartyInviteRequest);
        EXTRACT_MESSAGE(PartyAcceptInviteRequest);
        EXTRACT_MESSAGE(PartyLeaveRequest);
        EXTRACT_MESSAGE(CharacterTravelRequest);
        EXTRACT_MESSAGE(RequestActorValueChanges);
    }

    return UniquePtr<ClientMessage>(nullptr, &TiltedPhoques::Delete<ClientMessage>);
}
