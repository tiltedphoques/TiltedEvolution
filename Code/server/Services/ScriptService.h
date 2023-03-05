#pragma once

#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>
#include <TiltedCore/Lockable.hpp>

struct World;
struct ClientRpcCalls;
struct PlayerEnterWorldEvent;

namespace Resources
{
struct ResourceCollection;
}

struct ScriptService
{
    ScriptService(World& aWorld, entt::dispatcher& aDispatcher);
    ~ScriptService();

    TP_NOCOPYMOVE(ScriptService);

    void Initialize(Resources::ResourceCollection& aCollection) noexcept;
    bool LoadScript(const std::filesystem::path& aPath);

    std::tuple<bool, String> HandlePlayerJoin(const ConnectionId_t aEntity) noexcept;
    std::tuple<bool, String> HandleMove(const entt::entity aNpc) noexcept;
    std::tuple<bool, String> HandleChatMessage(const entt::entity aSender, const String& aMessage) noexcept;

    void HandlePlayerQuit(ConnectionId_t aConnectionId, Server::EDisconnectReason aReason) noexcept;

  protected:
    // void RegisterExtensions(ScriptContext& aContext) override;

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerEnterWorld(const PlayerEnterWorldEvent& acEvent) noexcept;
    // void BindStaticFunctions(ScriptContext& aContext) noexcept;
    // void BindTypes(ScriptContext& aContext) noexcept;

    void AddEventHandler(std::string acName, sol::function acFunction) noexcept;
    void CancelEvent(std::string aReason) noexcept;

    template <typename... Args>
    std::tuple<bool, String> CallCancelableEvent(const String& acName, Args&&... args) noexcept;

    template <typename... Args> void CallEvent(const String& acName, Args&&... args) noexcept;

  private:
    void BindInbuiltFunctions();

  private:
    using TCallbacks = Vector<sol::function>;

    World& m_world;
    bool m_eventCanceled{};
    String m_cancelReason;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_rpcCallsRequest;
    entt::scoped_connection m_playerEnterWorldConnection;

    // NOTE(Vince): keep in mind that cxx specifies construction and deconstruction order,
    // so do not touch this order of member variables
    TiltedPhoques::Lockable<sol::state, std::recursive_mutex> m_lua;
    Map<String, TCallbacks> m_callbacks;
    TiltedPhoques::Vector<sol::environment> m_sandboxes;
    sol::table m_globals{};
};

#include "ScriptService.inl"
