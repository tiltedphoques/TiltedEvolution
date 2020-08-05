#include <Services/ScriptService.h>
#include <Services/EnvironmentService.h>
#include <World.h>

#include <Scripts/Npc.h>
#include <Scripts/Player.h>

#include <Events/UpdateEvent.h>

#include <Messages/ClientRpcCalls.h>
#include <Messages/ServerScriptUpdate.h>

#include <Filesystem.hpp>
#include <Components.h>
#include <GameServer.h>

ScriptService::ScriptService(World& aWorld, entt::dispatcher& aDispatcher)
    : ScriptStore(true)
    , m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&ScriptService::OnUpdate>(this))
    , m_rpcCallsRequest(aDispatcher.sink<PacketEvent<ClientRpcCalls>>().connect<&ScriptService::OnRpcCalls>(this))
{
}

Vector<Script::Player> ScriptService::GetPlayers() const
{
    Vector<Script::Player> players;

    auto playerView = m_world.view<PlayerComponent>();
    for(auto entity : playerView)
    {
        players.push_back(Script::Player(entity, m_world));
    }

    return players;
}

Vector<Script::Npc> ScriptService::GetNpcs() const
{
    Vector<Script::Npc> npcs;

    auto npcView = m_world.view<CellIdComponent, MovementComponent, AnimationComponent, OwnerComponent>(entt::exclude<PlayerComponent>);
    for (auto entity : npcView)
    {
        npcs.push_back(Script::Npc(entity, m_world));
    }

    return npcs;
}

void ScriptService::Initialize() noexcept
{
    std::filesystem::path fullpath = TiltedPhoques::GetPath().string() + "\\scripts";
    LoadFullScripts(fullpath);
}

Scripts ScriptService::SerializeScripts() noexcept
{
    TiltedPhoques::Buffer buff(10000);
    Buffer::Writer writer(&buff);

    GetNetState()->SerializeDefinitions(writer);

    Scripts scripts;
    scripts.Data.assign(buff.GetData(), buff.GetData() + writer.Size());

    return scripts;
}

Objects ScriptService::GenerateDifferential() noexcept
{
    TiltedPhoques::Buffer buff(10000);
    Buffer::Writer writer(&buff);

    const auto ret = GetNetState()->GenerateDifferentialSnapshot(writer);
    if(ret)
    {
        Objects objects;
        objects.Data.assign(buff.GetData(), buff.GetData() + writer.Size());

        return objects;
    }

    return {};
}

FullObjects ScriptService::GenerateFull() noexcept
{
    TiltedPhoques::Buffer buff(10000);
    Buffer::Writer writer(&buff);

    GetNetState()->GenerateFullSnapshot(writer);

    FullObjects objects;
    objects.Data.assign(buff.GetData(), buff.GetData() + writer.Size());

    return objects;
}

std::tuple<bool, String> ScriptService::HandlePlayerConnect(const Script::Player& aPlayer) noexcept
{
    return CallCancelableEvent("onPlayerConnect", aPlayer);
}

std::tuple<bool, String> ScriptService::HandlePlayerEnterWorld(const Script::Player& aPlayer) noexcept
{
    return CallCancelableEvent("onPlayerEnterWorld", aPlayer);
}

std::tuple<bool, String> ScriptService::HandleMove(const Script::Npc& aNpc) noexcept
{
    return CallCancelableEvent("onCharacterMove", aNpc);
}

void ScriptService::HandlePlayerQuit(ConnectionId_t aConnectionId) noexcept
{
    CallEvent("onPlayerQuit", aConnectionId);
}

void ScriptService::RegisterExtensions(ScriptContext& aContext)
{
    ScriptStore::RegisterExtensions(aContext);

    BindTypes(aContext);
    BindStaticFunctions(aContext);
}

void ScriptService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    ServerScriptUpdate message;

    message.Data = GenerateDifferential();

    // Only send if the snapshot contains anything changed
    if(message.Data.IsEmpty() == false)
    {
        GameServer::Get()->SendToLoaded(message);       
    }

    CallEvent("onUpdate", acEvent.Delta);
}

void ScriptService::OnRpcCalls(const PacketEvent<ClientRpcCalls>& acRpcCalls) noexcept
{
    auto& data = acRpcCalls.Packet.Data;

    Buffer buff(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
    Buffer::Reader reader(&buff);

    GetNetState()->ProcessCallRequest(reader, acRpcCalls.ConnectionId);
}

void ScriptService::BindTypes(ScriptContext& aContext) noexcept
{
    using Script::Npc;
    using Script::Player;

    auto npcType = aContext.new_usertype<Npc>("Npc", sol::no_constructor);
    npcType["id"] = sol::readonly_property(&Npc::GetId);
    npcType["position"] = sol::readonly_property(&Npc::GetPosition);
    npcType["rotation"] = sol::readonly_property(&Npc::GetRotation);
    npcType["speed"] = sol::readonly_property(&Npc::GetSpeed);
    npcType["AddComponent"] = &Npc::AddComponent;

    auto playerType = aContext.new_usertype<Player>("Player", sol::no_constructor);
    playerType["id"] = sol::readonly_property(&Player::GetId);
    playerType["mods"] = sol::readonly_property(&Player::GetMods);
    playerType["ip"] = sol::readonly_property(&Player::GetIp);
    playerType["AddComponent"] = &Player::AddComponent;

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
    clockType["GetRealTime"] = []() { 
        auto t = std::time(nullptr);
        int h = (t / 3600) % 24;
        int m = (t / 60) % 60;
        return std::pair{h, m};
    };
}

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

void ScriptService::AddEventHandler(const std::string acName, const sol::function acFunction) noexcept
{
    m_callbacks[String(acName)].push_back(acFunction);
}

void ScriptService::CancelEvent(const std::string aReason) noexcept
{
    m_eventCanceled = true;
    m_cancelReason = aReason;
}
