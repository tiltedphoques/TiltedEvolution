
#include "Game/PlayerManager.h"
#include "GameServer.h"

namespace Script
{
namespace
{
void BindPlayer(sol::state_view aState)
{
    auto playerType = aState.new_usertype<Player>("Player", sol::constructors<Player(ConnectionId_t)>());

    playerType["GetId"] = &Player::GetId;
    playerType["GetConnectionId"] = &Player::GetConnectionId;
    playerType["GetCharacter"] = &Player::GetCharacter;
    playerType["GetParty"] = &Player::GetParty;
    playerType["GetUsername"] = &Player::GetUsername;
    playerType["GetEndPoint"] = &Player::GetEndPoint;
    playerType["GetDiscordId"] = &Player::GetDiscordId;
    playerType["GetStringCacheId"] = &Player::GetStringCacheId;
    playerType["GetLevel"] = &Player::GetLevel;
    // playerType["GetCellComponent"] = sol::overload(&Player::GetCellComponent, &Player::GetCellComponent);
    //  playerType["GetQuestLogComponent"] = sol::overload(&Player::GetQuestLogComponent,
    //  &Player::GetQuestLogComponent);
    playerType["SetDiscordId"] = &Player::SetDiscordId;
    playerType["SetEndpoint"] = &Player::SetEndpoint;
    playerType["SetUsername"] = &Player::SetUsername;
    playerType["SetMods"] = &Player::SetMods;
    playerType["SetModIds"] = &Player::SetModIds;
    playerType["SetCharacter"] = &Player::SetCharacter;
    playerType["SetStringCacheId"] = &Player::SetStringCacheId;
    playerType["SetLevel"] = &Player::SetLevel;
    playerType["SetCellComponent"] = &Player::SetCellComponent;
    playerType["Send"] = &Player::Send;
    playerType["IsPartyLeader"] = [](Player& aSelf) {
        return GameServer::Get()->GetWorld().GetPartyService().IsPlayerLeader(
            PlayerManager::Get()->GetByConnectionId(aSelf.GetConnectionId()));
    };
}

void BindPlayerManager(sol::state_view aState)
{
    auto table =
        aState.new_usertype<PlayerManager>("PlayerManager", sol::meta_function::construct, sol::no_constructor);
    table["get"] = []() -> PlayerManager& { return GameServer::Get()->GetWorld().GetPlayerManager(); };

    table["GetByConnectionId"] = [](PlayerManager& aSelf, uint32_t aConnID) -> Player const* {
        return aSelf.GetByConnectionId(aConnID);
    };
    table["GetById"] = [](PlayerManager& aSelf, uint32_t aConnID) -> Player const* { return aSelf.GetById(aConnID); };
    table["GetAllPlayers"] = [](PlayerManager& aSelf) {
        std::vector<Player*> allPlayers;
        aSelf.ForEach([&](Player* aPlayer) { allPlayers.push_back(aPlayer); });
        return allPlayers;
    };

    table["Count"] = &PlayerManager::Count;
}

} // namespace

void CreatePlayerBindings(sol::state_view aState)
{
    BindPlayer(aState);
    BindPlayerManager(aState);
}
} // namespace Script
