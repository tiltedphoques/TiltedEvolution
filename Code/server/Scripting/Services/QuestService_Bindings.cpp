#include "GameServer.h"

namespace Script
{
void CreateQuestServiceBindings(sol::state_view aState)
{
    auto questType =
        aState.new_usertype<QuestService>("QuestService", sol::meta_function::construct, sol::no_constructor);

    questType["get"] = []() -> QuestService& { return GameServer::Get()->GetWorld().GetQuestService(); };
}
} // namespace Script
