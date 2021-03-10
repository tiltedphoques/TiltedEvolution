#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include <Messages/ServerMessageFactory.h>

#include <Messages/AuthenticationResponse.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ServerScriptUpdate.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/NotifyQuestUpdate.h>
#include <Messages/NotifyPlayerList.h>
#include <Messages/NotifyPartyInfo.h>
#include <Messages/NotifyPartyInvite.h>
#include <Messages/NotifyCharacterTravel.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Messages/NotifySpawnData.h>
#include <Messages/NotifyDeathStateChange.h>

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
        EXTRACT_MESSAGE(ServerTimeSettings);
        EXTRACT_MESSAGE(CharacterSpawnRequest);
        EXTRACT_MESSAGE(NotifyInventoryChanges);
        EXTRACT_MESSAGE(NotifyFactionsChanges);
        EXTRACT_MESSAGE(NotifyRemoveCharacter);
        EXTRACT_MESSAGE(NotifyQuestUpdate);
        EXTRACT_MESSAGE(NotifyPlayerList);
        EXTRACT_MESSAGE(NotifyPartyInfo);
        EXTRACT_MESSAGE(NotifyPartyInvite);
        EXTRACT_MESSAGE(NotifyCharacterTravel);
        EXTRACT_MESSAGE(NotifyActorValueChanges);
        EXTRACT_MESSAGE(NotifyActorMaxValueChanges);
        EXTRACT_MESSAGE(NotifyHealthChangeBroadcast);
        EXTRACT_MESSAGE(NotifySpawnData);
        EXTRACT_MESSAGE(NotifyDeathStateChange);
    }

    return UniquePtr<ServerMessage>(nullptr);
}
