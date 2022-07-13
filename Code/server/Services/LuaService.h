#pragma once
#include <optional>
#include <server/Events/PacketEvent.h>

struct World;
struct PlayerJoinEvent;
struct PlayerLeaveEvent;
struct RequestInventoryChanges;

/**
 * @brief Exposes server-side events and methods to Lua scripting
 */
struct LuaService
{
    LuaService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~LuaService() noexcept = default;

    TP_NOCOPYMOVE(LuaService);

  private:
    void OnPlayerJoin(const PlayerJoinEvent& acMessage) const noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acMessage) const noexcept;
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) const noexcept;

    void InitializeCETLikeScripting() noexcept;

    std::optional<sol::function> GetFunction(const char* aEventName) const noexcept;

    World& m_world;

    entt::scoped_connection m_playerJoinConnection;
    entt::scoped_connection m_playerLeaveConnection;
    entt::scoped_connection m_notifyInventoryChangesConnection;

    sol::state m_lua;
};
