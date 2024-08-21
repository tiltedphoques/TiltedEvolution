#include "GameServer.h"

namespace Script
{
void CreateCharacterServiceBindings(sol::state_view aState)
{
    auto characterType =
        aState.new_usertype<CharacterService>("CharacterService", sol::meta_function::construct, sol::no_constructor);

    characterType["get"] = []() -> CharacterService& { return GameServer::Get()->GetWorld().GetCharacterService(); };
}
} // namespace Script
