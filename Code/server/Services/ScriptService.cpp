#include <stdafx.h>

#include <Services/ScriptService.h>
#include <Services/CalendarService.h>
#include <World.h>

#include <Scripting/Npc.h>
#include <Scripting/Player.h>
#include <Scripting/Quest.h>
#include <Scripting/Party.h>

#include <Events/UpdateEvent.h>
#include <Events/PlayerEnterWorldEvent.h>

#include <TiltedCore/Filesystem.hpp>
#include <Components.h>
#include <GameServer.h>

#include <resources/ResourceCollection.h>

ScriptService::ScriptService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&ScriptService::OnUpdate>(this))
    , m_playerEnterWorldConnection(aDispatcher.sink<PlayerEnterWorldEvent>().connect<&ScriptService::OnPlayerEnterWorld>(this))
{
}

void ScriptService::Initialize(Resources::ResourceCollection& aCollection) noexcept
{
    auto lua = m_lua.Lock();
    auto& luaVm = lua.Get();
    m_globals = luaVm.globals();
    
    luaVm.open_libraries(sol::lib::base, sol::lib::string, sol::lib::io, sol::lib::math, sol::lib::package,
                         sol::lib::os, sol::lib::table, sol::lib::bit32);

    // make sure to set package path to current directory scope
    // as this could get overriden by LUA_PATH environment variable
    luaVm["package"]["path"] = "./?.lua";

    BindInbuiltFunctions();
    
    aCollection.ForEachManifest([&](const Resources::Manifest001& aManifest) {
        if (aManifest.EntryPoint.empty()) // just a dependency?
            return;
        auto entryPointPath = aCollection.GetResourceFolderPath() / aManifest.FolderName / aManifest.EntryPoint;
        if (!std::filesystem::exists(entryPointPath))
        {
            spdlog::warn("Script entry point {} does not exist", entryPointPath.string());
            return;
        }
        LoadScript(entryPointPath);
    });
}

bool ScriptService::LoadScript(const std::filesystem::path& aPath)
{
    try
    {
        auto lua = m_lua.Lock();
        auto& luaVm = lua.Get();
        
        const auto result = luaVm.script_file(aPath.string());

        if (!result.valid())
        {
            const sol::error error = result;
            spdlog::error("Failed to load script file: {}", error.what());
            return false;
        }
    }
    catch (std::exception& exception)
    {
        spdlog::error("Failed to load script file: {}", exception.what());
        return false;
    }

    return true;
}

void ScriptService::BindInbuiltFunctions()
{
    auto lua = m_lua.Lock();
    auto& luaVm = lua.Get();

    // git build information
    {
        auto table = luaVm.create_named_table("BuildInfo");
        table["Commit"] = BUILD_COMMIT;
        table["Branch"] = BUILD_BRANCH;
    }


    // https://github.com/tiltedphoques/TiltedRevolution/blob/master/Code/server/Services/ScriptService.cpp
    {
        luaVm.set_function("addEventHandler", [this](std::string acName, sol::function aFunction) {
            AddEventHandler(std::move(acName), std::move(aFunction));
        });
        luaVm.set_function("cancelEvent", [this](std::string acReason) { CancelEvent(std::move(acReason)); });
    }

    // game time information
    {
        auto cal = luaVm.new_usertype<CalendarService>("Calendar", sol::no_constructor);
        //cal["GetGameTime"] = &CalendarService::GetGameTime;
    }

    {
        auto playerType = luaVm.new_usertype<Script::Player>("Player", sol::no_constructor);
        playerType["id"] = sol::readonly_property(&Script::Player::GetId);
        playerType["discordId"] = sol::readonly_property(&Script::Player::GetDiscordId);
        playerType["party"] = sol::readonly_property(&Script::Player::GetParty);
        playerType["name"] = sol::readonly_property(&Script::Player::GetName);
    }

    {
        auto worldType = luaVm.new_usertype<World>("World", sol::no_constructor);
        worldType["get"] = [this]() { return &m_world; };
        //worldType["npcs"] = sol::readonly_property([this]() { return GetNpcs(); });
        worldType["players"] = sol::readonly_property([this]() { return GetPlayers(); });
    }

    {
        auto upTime = luaVm.new_usertype<GameServer::UpTime>("UpTime", sol::no_constructor);
        upTime["weeks"] = sol::readonly_property(&GameServer::UpTime::GetWeeks);
        upTime["days"] = sol::readonly_property(&GameServer::UpTime::GetDays);
        upTime["hours"] = sol::readonly_property(&GameServer::UpTime::GetHours);
        upTime["minutes"] = sol::readonly_property(&GameServer::UpTime::GetMintutes);
        
  
        auto server = luaVm.new_usertype<GameServer>("GameServer", sol::no_constructor);
        server["get"] = [this]() { return GameServer::Get(); };
        upTime["name"] = sol::readonly_property([]() { return GameServer::Get()->GetInfo().name; });
        upTime["tags"] = sol::readonly_property([]() { return GameServer::Get()->GetInfo().tagList; });
        upTime["tickrate"] = sol::readonly_property([]() { return GameServer::Get()->GetInfo().tick_rate; });
        server["GetUptime"] = &GameServer::GetUptime;
        server["Close"] = &GameServer::Kill;
    }

    {
        auto server = luaVm.new_usertype<GameServer>("Chat", sol::no_constructor);
        server["get"] = [this]() { return GameServer::Get(); };
        upTime["name"] = sol::readonly_property([]() { return GameServer::Get()->GetInfo().name; });

        GameServer::Get()->SendToPlayers(notifyMessage);

    }


Vector<Script::Player> ScriptService::GetPlayers() const
{
    Vector<Script::Player> players;

    auto &playerManager = m_world.GetPlayerManager();
    playerManager.ForEach(
        [&](const Player* aPlayer) { players.emplace_back(Script::Player(aPlayer->GetId(), *aPlayer->GetCharacter(), m_world)); });

    return players;
}

Vector<Script::Npc> ScriptService::GetNpcs() const
{
    Vector<Script::Npc> npcs;

    /* auto npcView = m_world.view<CellIdComponent, MovementComponent, AnimationComponent, OwnerComponent>(
        entt::exclude<PlayerComponent>);
    for (auto entity : npcView)
    {
        npcs.push_back(Script::Npc(entity, m_world));
    }*/

    return npcs;
}

std::tuple<bool, String> ScriptService::HandleMove(const Script::Npc& aNpc) noexcept
{
    return CallCancelableEvent("onCharacterMove", aNpc);
}

std::tuple<bool, String> ScriptService::HandlePlayerJoin(const Script::Player& aPlayer) noexcept
{
    return CallCancelableEvent("onPlayerJoin", aPlayer);
}

void ScriptService::HandlePlayerQuit(ConnectionId_t aConnectionId, Server::EDisconnectReason aReason) noexcept
{
    std::string reason;

    switch (aReason)
    {
    case Server::EDisconnectReason::Quit: reason = "Quit"; break;
    case Server::EDisconnectReason::Kicked: reason = "Kicked"; break;
    case Server::EDisconnectReason::Banned: reason = "Banned"; break;
    case Server::EDisconnectReason::BadConnection: reason = "Connection lost"; break;
    case Server::EDisconnectReason::TimedOut: reason = "Timed out"; break;
    case Server::EDisconnectReason::Unknown:
    default: reason = "Unknown"; break;
    }

    CallEvent("onPlayerQuit", aConnectionId, reason);
}

void ScriptService::HandleQuestStart(const Script::Player& aPlayer, const Script::Quest& aQuest) noexcept
{
    CallEvent("onQuestStart", aPlayer, aQuest);
}

void ScriptService::HandleQuestStage(const Script::Player& aPlayer, const Script::Quest& aQuest) noexcept
{
    CallEvent("onQuestStage", aPlayer, aQuest);
}

void ScriptService::HandleQuestStop(const Script::Player& aPlayer, uint32_t aformId) noexcept
{
    CallEvent("onQuestStop", aPlayer, aformId);
}

#if 0
void ScriptService::RegisterExtensions(ScriptContext& aContext)
{
    ScriptStore::RegisterExtensions(aContext);

    BindTypes(aContext);
    BindStaticFunctions(aContext);
}
#endif

void ScriptService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    CallEvent("onUpdate", acEvent.Delta);
}

void ScriptService::OnPlayerEnterWorld(const PlayerEnterWorldEvent& acEvent) noexcept
{
    /* const Script::Player cPlayer(acEvent.Entity, m_world);

    CallEvent("onPlayerEnterWorld", cPlayer);*/
}
#if 0
void ScriptService::BindTypes(ScriptContext& aContext) noexcept
{
    using Script::Npc;
    using Script::Player;
    using Script::Quest;
    using Script::Party;

    auto npcType = aContext.new_usertype<Npc>("Npc", sol::no_constructor);
    npcType["id"] = sol::readonly_property(&Npc::GetId);
    npcType["position"] = sol::readonly_property(&Npc::GetPosition);
    npcType["rotation"] = sol::readonly_property(&Npc::GetRotation);
    npcType["speed"] = sol::readonly_property(&Npc::GetSpeed);

    auto playerType = aContext.new_usertype<Player>("Player", sol::no_constructor);
    playerType["id"] = sol::readonly_property(&Player::GetId);
    playerType["mods"] = sol::readonly_property(&Player::GetMods);
    playerType["ip"] = sol::readonly_property(&Player::GetIp);
    playerType["party"] = sol::readonly_property(&Player::GetParty);
    playerType["discordid"] = sol::readonly_property(&Player::GetDiscordId);
    playerType["AddQuest"] = &Player::AddQuest;
    playerType["GetQuests"] = &Player::GetQuests;
    playerType["RemoveQuest"] = &Player::RemoveQuest; 

    auto questType = aContext.new_usertype<Quest>("Quest", sol::no_constructor);
    questType["id"] = sol::readonly_property(&Quest::GetId);
    questType["GetStage"] = &Quest::GetStage;
    // questType["SetStage"] = &Quest::SetStage;    

    auto partyType = aContext.new_usertype<Party>("Party", sol::no_constructor);
    partyType["id"] = sol::readonly_property(&Party::GetId);
    partyType["members"] = sol::readonly_property(&Party::GetPlayers);

    auto worldType = aContext.new_usertype<World>("World", sol::no_constructor);
    worldType["get"] = [this]() { return &m_world; };
    worldType["npcs"] = sol::readonly_property([this]() { return GetNpcs(); });
    worldType["players"] = sol::readonly_property([this]() { return GetPlayers(); });

    auto clockType = aContext.new_usertype<EnvironmentService>("Clock", sol::no_constructor);
    clockType["get"] = [this]() { return &m_world.GetEnvironmentService(); };
    clockType["SetTime"] = &EnvironmentService::SetTime;
    clockType["GetTime"] = &EnvironmentService::GetTime;
    clockType["GetDate"] = &EnvironmentService::GetDate;
    clockType["GetTimeScale"] = &EnvironmentService::GetTimeScale;
    clockType["GetRealTime"] = &EnvironmentService::GetRealTime;
}
#endif

void ScriptService::AddEventHandler(const std::string acName, const sol::function acFunction) noexcept
{
    m_callbacks[String(acName)].push_back(acFunction);
}

void ScriptService::CancelEvent(const std::string aReason) noexcept
{
    m_eventCanceled = true;
    m_cancelReason = aReason;
}
