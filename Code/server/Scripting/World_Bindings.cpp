#include "GameServer.h"
#include "World.h"

namespace Script
{
void CreateWorldBindings(sol::state_view aState)
{
    auto type =
        aState.new_usertype<World>("World", sol::meta_function::construct, sol::no_constructor);

    type["get"] = []() -> World& { return GameServer::Get()->GetWorld(); };
}
} // namespace Script
