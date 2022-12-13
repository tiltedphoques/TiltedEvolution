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

ScriptService::ScriptService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&ScriptService::OnUpdate>(this))
    , m_playerEnterWorldConnection(aDispatcher.sink<PlayerEnterWorldEvent>().connect<&ScriptService::OnPlayerEnterWorld>(this))
{
}

Vector<Script::Player> ScriptService::GetPlayers() const
{
    Vector<Script::Player> players;

    /* auto playerView = m_world.view<PlayerComponent>();
    for(auto entity : playerView)
    {
        players.push_back(Script::Player(entity, m_world));
    }*/

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

void ScriptService::Initialize() noexcept
{
    auto path = TiltedPhoques::GetPath() / "scripts"; 
    //LoadFullScripts(path);
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
    case Server::EDisconnectReason::Quit:
        reason = "Quit";
        break;
    case Server::EDisconnectReason::Kicked:
        reason = "Kicked";
        break;
    case Server::EDisconnectReason::Banned:
        reason = "Banned";
        break;
    case Server::EDisconnectReason::BadConnection:
        reason = "Connection lost";
        break;
    case Server::EDisconnectReason::TimedOut:
        reason = "Timed out";
        break;
    case Server::EDisconnectReason::Unknown:
    default:
        reason = "Unknown";
        break;
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

#if 0
void ScriptService::BindStaticFunctions(ScriptContext& aContext) noexcept
{
    aContext.set_function("addEventHandler", [this](std::string acName, sol::function aFunction) 
    { 
        AddEventHandler(std::move(acName), std::move(aFunction)); 
    });
    aContext.set_function("cancelEvent", [this](std::string acReason)
    { 
        CancelEvent(std::move(acReason)); 
    });
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
