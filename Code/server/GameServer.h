#pragma once

#include <World.h>
#include <Messages/Message.h>
#include <Messages/AuthenticationRequest.h>
#include <AdminMessages/Message.h>

using TiltedPhoques::String;
using TiltedPhoques::Server;
using TiltedPhoques::ConnectionId_t;

struct AuthenticationRequest;

namespace Console
{
class ConsoleRegistry;
}

struct GameServer final : Server
{
public:
    // TODO: eventually refactor this.
    struct Info
    {
        String name;
        String desc;
        String icon_url;
        String tagList;
        uint16_t tick_rate;
    };

    GameServer(Console::ConsoleRegistry * apConsole) noexcept;
    virtual ~GameServer();

    TP_NOCOPYMOVE(GameServer);

    static GameServer* Get() noexcept;

    void Initialize();
    void Kill();

    void BindMessageHandlers();
    void BindServerCommands();

    void UpdateInfo();

    // Implement TiltedPhoques::Server
    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId_t aConnectionId) override;
    void OnConnection(ConnectionId_t aHandle) override;
    void OnDisconnection(ConnectionId_t aConnectionId, EDisconnectReason aReason) override;

    // Packet dispatching
    void Send(ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const;
    void Send(ConnectionId_t aConnectionId, const ServerAdminMessage& acServerMessage) const;
    void SendToLoaded(const ServerMessage& acServerMessage) const;
    void SendToPlayers(const ServerMessage& acServerMessage) const;
    void SendToPlayersInRange(const ServerMessage& acServerMessage, const entt::entity acOrigin) const;

    const Info& GetInfo() const noexcept
    {
        return m_info;
    }

    bool IsRunning() const noexcept { return !m_requestStop; }

    template<class T>
    void ForEachAdmin(const T& aFunctor)
    {
        for (auto id : m_adminSessions)
            aFunctor(id);
    }

protected:

    void HandleAuthenticationRequest(ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest);

private:

    void UpdateTitle() const;


private:
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::function<void(UniquePtr<ClientMessage>&, ConnectionId_t)> m_messageHandlers[kClientOpcodeMax];
    std::function<void(UniquePtr<ClientAdminMessage>&, ConnectionId_t)> m_adminMessageHandlers[kClientAdminOpcodeMax];

    Info m_info{};
    std::unique_ptr<World> m_pWorld;
    Console::ConsoleRegistry* m_pCommands;

    Set<ConnectionId_t> m_adminSessions;
    Map<ConnectionId_t, entt::entity> m_connectionToEntity;

    bool m_requestStop;

    static GameServer* s_pInstance;
};
