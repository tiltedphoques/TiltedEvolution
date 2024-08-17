#include "GameServer.h"

namespace Script
{
void CreateScriptServiceBindings(sol::state_view aState)
{
    auto scriptType =
        aState.new_usertype<ScriptService>("ScriptService", sol::meta_function::construct, sol::no_constructor);

    scriptType["get"] = []() -> ScriptService& { return GameServer::Get()->GetWorld().GetScriptService(); };
}
} // namespace Script
