
#include "Game/Cell.h"
#include "Game/Player.h"
#include "Game/PlayerManager.h"

#include <World.h>

#include "Components/ActorValuesComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/CellIdComponent.h"
#include "Components/CharacterComponent.h"
#include "Components/FormIdComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ModsComponent.h"
#include "Components/MovementComponent.h"
#include "Components/ObjectComponent.h"
#include "Components/OwnerComponent.h"
#include "Components/PartyComponent.h"
#include "Components/QuestLogComponent.h"

namespace Script
{
void BindMetadata(sol::state_view aState)
{
    auto table = aState.create_named_table("BuildInfo");
    table["Commit"] = BUILD_COMMIT;
    table["Branch"] = BUILD_BRANCH;
}

void BindEnttEntity(sol::state_view aState)
{
    auto tab = aState.new_usertype<entt::entity>("entity");
}

void BindGameId(sol::state& aState)
{
    auto table = aState.new_usertype<GameId>("GameId", sol::constructors<GameId(), GameId(uint32_t, uint32_t)>());
    table["BaseId"] = &GameId::BaseId;
    table["ModId"] = &GameId::ModId;
    table[sol::meta_function::equal_to] = &GameId::operator==;
}

void CreateMathBindings(sol::state_view);
void CreatePlayerBindings(sol::state_view);
void CreateComponentBindings(sol::state_view);
void CreateGameServerBindings(sol::state_view);
void CreateServicesBindings(sol::state_view);
void CreateWorldBindings(sol::state_view);

sol::table BindModsComponent(sol::state_view aState)
{
    sol::table module = aState.create_table();

    auto modsComponentType = aState.new_usertype<ModsComponent>("ModsComponent");
#if 0
    auto entryType = aState.new_usertype<ModsComponent::Entry>(
        "Entry", sol::constructors<ModsComponent::Entry(uint32_t, uint32_t)>());

    // Bind the public methods of ModsComponent
    modsComponentType["AddStandard"] = &ModsComponent::AddStandard;
    modsComponentType["AddLite"] = &ModsComponent::AddLite;
    modsComponentType["AddServerMod"] = &ModsComponent::AddServerMod;
    modsComponentType["GetStandardMods"] = &ModsComponent::GetStandardMods;
    modsComponentType["GetLiteMods"] = &ModsComponent::GetLiteMods;
    modsComponentType["GetServerMods"] = &ModsComponent::GetServerMods;
    modsComponentType["IsInstalled"] = &ModsComponent::IsInstalled;
#endif

#if 0
    // Bind the TModList type
    auto modListType = aState.new_usertype<ModsComponent::TModList>("TModList");
    modListType[sol::meta_function::index] = [](const ModsComponent::TModList& modList,
                                                const String& key) -> const ModsComponent::Entry* {
        const auto it = modList.find(key);
        return it == modList.cend() ? nullptr : &it->second;
    };

    // Bind the Entry type
    entryType["id"] = &ModsComponent::Entry::id;
    entryType["refCount"] = &ModsComponent::Entry::refCount;
#endif

    return module;
}


void CreateScriptBindings(sol::state& aState)
{
    BindMetadata(aState);

    // core primitives
    BindEnttEntity(aState);
    BindGameId(aState);

    // core math primitives
    CreateMathBindings(aState);

    // entity
    CreatePlayerBindings(aState);
    CreateComponentBindings(aState);

    CreateGameServerBindings(aState);
    CreateServicesBindings(aState);
    CreateWorldBindings(aState);
    BindModsComponent(aState);
}
} // namespace Script
