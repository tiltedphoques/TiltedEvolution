#include "GameServer.h"

namespace Script
{
void CreatePartyServiceBindings(sol::state_view aState)
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
} // namespace Script
