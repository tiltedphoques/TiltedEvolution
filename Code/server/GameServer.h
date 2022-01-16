#pragma once

#include <AdminMessages/Message.h>
#include <Messages/AuthenticationRequest.h>
#include <Messages/Message.h>
#include <World.h>

#include <server_plugins/PluginList.h>

using TiltedPhoques::ConnectionId_t;
using TiltedPhoques::Server;
using TiltedPhoques::String;

struct AuthenticationRequest;

struct GameServer final : Server
{
  public:
    struct Info
    {
        String name;
        String desc;
        String icon_url;
        uint16_t tick_rate;
    };

    GameServer() noexcept;
    virtual ~GameServer();

    TP_NOCOPYMOVE(GameServer);

    void Initialize();
    void BindMessageHandlers();
    void UpdateInfo();

    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId_t aConnectionId) override;
    void OnConnection(ConnectionId_t aHandle) override;
    void OnDisconnection(ConnectionId_t aConnectionId, EDisconnectReason aReason) override;

    void Send(ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const;
    void Send(ConnectionId_t aConnectionId, const ServerAdminMessage& acServerMessage) const;
    void SendToLoaded(const ServerMessage& acServerMessage) const;
    void SendToPlayers(const ServerMessage& acServerMessage) const;
    void SendToPlayersInRange(const ServerMessage& acServerMessage, const entt::entity acOrigin) const;

    const Info& GetInfo() const noexcept
    {
        return m_info;
    }

    void Stop() noexcept;

    static GameServer* Get() noexcept;

    template <class T> void ForEachAdmin(const T& aFunctor)
    {
        for (auto id : m_adminSessions)
            aFunctor(id);
    }

  private:
    void UpdateTitle() const;

  protected:
    void HandleAuthenticationRequest(ConnectionId_t aConnectionId,
                                     const UniquePtr<AuthenticationRequest>& acRequest) noexcept;

  private:
    static GameServer* s_pInstance;

    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::function<void(UniquePtr<ClientMessage>&, ConnectionId_t)> m_messageHandlers[kClientOpcodeMax];
    std::function<void(UniquePtr<ClientAdminMessage>&, ConnectionId_t)> m_adminMessageHandlers[kClientAdminOpcodeMax];

    Info m_info{};
    std::unique_ptr<World> m_pWorld;

    Set<ConnectionId_t> m_adminSessions;
    Map<ConnectionId_t, entt::entity> m_connectionToEntity;
    bool m_requestStop;

    server_plugins::PluginList m_pluginList;
    std::vector<server_plugins::Plugin*> m_pluginInstances;
};
