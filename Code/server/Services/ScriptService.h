#pragma once

#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>
#include <TiltedCore/Lockable.hpp>

struct World;
struct ClientRpcCalls;
struct PlayerEnterWorldEvent;

namespace Script
{
struct Npc;
struct Player;
struct Quest;
} // namespace Script

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

    std::tuple<bool, String> HandlePlayerJoin(const Script::Player& aPlayer) noexcept;
    std::tuple<bool, String> HandleMove(const Script::Npc& aNpc) noexcept;
    // bool HandleChatMessage();

    void HandlePlayerQuit(ConnectionId_t aConnectionId, Server::EDisconnectReason aReason) noexcept;

    void HandleQuestStart(const Script::Player& aPlayer, const Script::Quest& aQuest) noexcept;
    void HandleQuestStage(const Script::Player& aPlayer, const Script::Quest& aQuest) noexcept;
    void HandleQuestStop(const Script::Player& aPlayer, uint32_t aformId) noexcept;

  protected:
    // void RegisterExtensions(ScriptContext& aContext) override;

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerEnterWorld(const PlayerEnterWorldEvent& acEvent) noexcept;
    // void BindStaticFunctions(ScriptContext& aContext) noexcept;
    // void BindTypes(ScriptContext& aContext) noexcept;

    void AddEventHandler(std::string acName, sol::function acFunction) noexcept;
    void CancelEvent(std::string aReason) noexcept;

    [[nodiscard]] Vector<Script::Player> GetPlayers() const;
    [[nodiscard]] Vector<Script::Npc> GetNpcs() const;

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
