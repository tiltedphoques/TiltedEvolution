#include "GameServer.h"
#include "World.h"

namespace Script
{
void BindCalendarService(sol::state_view aState)
{
    auto calendarType =
        aState.new_usertype<CalendarService>("CalendarService", sol::meta_function::construct, sol::no_constructor);

    calendarType["get"] = []() -> CalendarService& { return GameServer::Get()->GetWorld().GetCalendarService(); };
    calendarType["GetTimeScale"] = []() { return GameServer::Get()->GetWorld().GetCalendarService().GetTimeScale(); };
}

void BindPartyService(sol::state_view aState)
{
    auto partyType =
        aState.new_usertype<PartyService>("PartyService", sol::meta_function::construct, sol::no_constructor);

    partyType["get"] = []() -> PartyService& { return GameServer::Get()->GetWorld().GetPartyService(); };
    partyType["IsPlayerInParty"] = [](PartyService& aService, uint32_t aConnID) -> bool {
        Player* player = PlayerManager::Get()->GetByConnectionId(aConnID);
        if (player == nullptr)
            return false;
        return aService.IsPlayerInParty(player);
    };
}

void BindCharacterService(sol::state_view aState)
{
    auto characterType =
        aState.new_usertype<CharacterService>("CharacterService", sol::meta_function::construct, sol::no_constructor);

    characterType["get"] = []() -> CharacterService& { return GameServer::Get()->GetWorld().GetCharacterService(); };
}

void BindPlayerService(sol::state_view aState)
{
    auto playerType =
        aState.new_usertype<PlayerService>("PlayerService", sol::meta_function::construct, sol::no_constructor);
    
    playerType["get"] = []() -> PlayerService& { return GameServer::Get()->GetWorld().GetPlayerService(); };
}

void BindQuestService(sol::state_view aState)
{
    auto questType =
        aState.new_usertype<QuestService>("QuestService", sol::meta_function::construct, sol::no_constructor);
    
    questType["get"] = []() -> QuestService& { return GameServer::Get()->GetWorld().GetQuestService(); };
}

void BindScriptService(sol::state_view aState)
{
    auto scriptType =
        aState.new_usertype<ScriptService>("ScriptService", sol::meta_function::construct, sol::no_constructor);
    
    scriptType["get"] = []() -> ScriptService& { return GameServer::Get()->GetWorld().GetScriptService(); };
}

void CreateServicesBindings(sol::state_view aState)
{
    BindCalendarService(aState);
    BindPartyService(aState);
    BindCharacterService(aState);
    BindPlayerService(aState);
    BindQuestService(aState);
    BindScriptService(aState);
}
} // namespace Script
