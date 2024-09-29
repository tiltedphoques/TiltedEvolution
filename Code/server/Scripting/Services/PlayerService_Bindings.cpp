#include "GameServer.h"

namespace Script
{
void CreatePlayerServiceBindings(sol::state_view aState)
{
    auto playerType =
        aState.new_usertype<PlayerService>("PlayerService", sol::meta_function::construct, sol::no_constructor);

    playerType["get"] = []() -> PlayerService& { return GameServer::Get()->GetWorld().GetPlayerService(); };
}
} // namespace Script
