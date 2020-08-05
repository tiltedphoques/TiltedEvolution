#pragma once

#include <Events/UpdateEvent.h>
#include <ScriptStore.h>
#include <Events/PacketEvent.h>

#include <Structs/Objects.h>
#include <Structs/FullObjects.h>
#include <Structs/Scripts.h>

struct World;
struct ClientRpcCalls;

namespace Script
{
    struct Npc;
    struct Player;
}

struct ScriptService : ScriptStore
{
    ScriptService(World& aWorld, entt::dispatcher& aDispatcher);
    ~ScriptService() noexcept = default;

    TP_NOCOPYMOVE(ScriptService);

    void Initialize() noexcept;
    Scripts SerializeScripts() noexcept;
    Objects GenerateDifferential() noexcept;
    FullObjects GenerateFull() noexcept;

    std::tuple<bool, String> HandlePlayerConnect(const Script::Player& aPlayer) noexcept;
    std::tuple<bool, String> HandlePlayerEnterWorld(const Script::Player& aPlayer) noexcept;
    std::tuple<bool, String> HandleMove(const Script::Npc& aNpc) noexcept;

    void HandlePlayerQuit(ConnectionId_t aConnectionId) noexcept;

protected:

    void RegisterExtensions(ScriptContext& aContext) override;

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnRpcCalls(const PacketEvent<ClientRpcCalls>& acRpcCalls) noexcept;

    void BindStaticFunctions(ScriptContext& aContext) noexcept;
    void BindTypes(ScriptContext& aContext) noexcept;

    void AddEventHandler(std::string acName, sol::function acFunction) noexcept;
    void CancelEvent(std::string aReason) noexcept;

    [[nodiscard]] Vector<Script::Player> GetPlayers() const;
    [[nodiscard]] Vector<Script::Npc> GetNpcs() const;

    template<typename... Args>
    std::tuple<bool, String> CallCancelableEvent(const String& acName, Args&&... args);

    template<typename... Args>
    void CallEvent(const String& acName, Args&&... args);

private:


    using TCallbacks = Vector<sol::function>;

    World& m_world;

    bool m_eventCanceled{};
    String m_cancelReason;
    Map<String, TCallbacks> m_callbacks;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_rpcCallsRequest;
};

#include "ScriptService.inl"
