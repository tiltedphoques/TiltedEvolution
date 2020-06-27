#pragma once

#include <Events/UpdateEvent.h>
#include <ScriptStore.h>
#include <Events/PacketEvent.h>

struct World;

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

    Vector<uint8_t> SerializeScripts() noexcept;
    Vector<uint8_t> GenerateDifferential() noexcept;
    Vector<uint8_t> GenerateFull() noexcept;

    std::tuple<bool, String> HandlePlayerConnect(const Script::Player& aPlayer) noexcept;
    std::tuple<bool, String> HandlePlayerEnterWorld(const Script::Player& aPlayer) noexcept;
    std::tuple<bool, String> HandleMove(const Script::Npc& aNpc) noexcept;

    void HandlePlayerQuit(ConnectionId_t aConnectionId) noexcept;

protected:

    void RegisterExtensions(ScriptContext& aContext) override;

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnRpcCalls(const PacketEvent<TiltedMessages::RpcCallsRequest>& acRpcCalls) noexcept;

    void BindStaticFunctions(ScriptContext& aContext) noexcept;
    void BindTypes(ScriptContext& aContext) noexcept;

    void AddEventHandler(std::string acName, sol::function acFunction) noexcept;
    void CancelEvent(std::string aReason) noexcept;

    template<typename... Args>
    std::tuple<bool, String> CallCancelableEvent(const String& acName, Args&&... args)
    {
        m_eventCanceled = false;

        auto& callbacks = m_callbacks[acName];

        for (auto& callback : callbacks)
        {
            auto result = callback(std::forward<Args>(args)...);

            if (!result.valid())
            {
                sol::error err = result;
                spdlog::error(err.what());
            }

            if (m_eventCanceled == true)
                return std::make_tuple(true, m_cancelReason);
        }

        return std::make_tuple(false, "");
    }

    template<typename... Args>
    void CallEvent(const String& acName, Args&&... args)
    {
        auto& callbacks = m_callbacks[acName];

        for (auto& callback : callbacks)
        {
            auto result = callback(std::forward<Args>(args)...);
            if (!result.valid())
            {
                sol::error err = result;
                spdlog::error(err.what());
            }
        }
    }

    [[nodiscard]] Vector<Script::Player> GetPlayers() const;
    [[nodiscard]] Vector<Script::Npc> GetNpcs() const;

private:

    void Initialize() noexcept;

    using TCallbacks = Vector<sol::function>;

    World& m_world;

    bool m_eventCanceled{};
    String m_cancelReason;
    Map<String, TCallbacks> m_callbacks;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_rpcCallsRequest;
};
