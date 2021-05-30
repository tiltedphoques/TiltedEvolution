#pragma once

#include <World.h>
#include <Messages/Message.h>
#include <Messages/AuthenticationRequest.h>
#include <AdminMessages/Message.h>

using TiltedPhoques::String;
using TiltedPhoques::Server;
using TiltedPhoques::ConnectionId_t;

struct AuthenticationRequest;

struct GameServer final : Server
{
    GameServer(uint16_t aPort, bool aPremium, String aName, String aToken, String aAdminPassword = "") noexcept;
    virtual ~GameServer();

    TP_NOCOPYMOVE(GameServer);

    void Initialize();

    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId_t aConnectionId) override;
    void OnConnection(ConnectionId_t aHandle) override;
    void OnDisconnection(ConnectionId_t aConnectionId, EDisconnectReason aReason) override;

    void Send(ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const;
    void Send(ConnectionId_t aConnectionId, const ServerAdminMessage& acServerMessage) const;
    void SendToLoaded(const ServerMessage& acServerMessage) const;
    void SendToPlayers(const ServerMessage& acServerMessage) const;

    const String& GetName() const noexcept;

    void Stop() noexcept;

    static GameServer* Get() noexcept;

protected:

    void HandleAuthenticationRequest(ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest) noexcept;

private:

    void SetTitle() const;

    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::function<void(UniquePtr<ClientMessage>&, ConnectionId_t)> m_messageHandlers[kClientOpcodeMax];
    std::function<void(UniquePtr<ClientAdminMessage>&, ConnectionId_t)> m_adminMessageHandlers[kClientAdminOpcodeMax];

    String m_name;
    String m_token;
    String m_adminPassword;

    std::unique_ptr<World> m_pWorld;

    Set<ConnectionId_t> m_adminSessions;
    Map<ConnectionId_t, entt::entity> m_connectionToEntity;

    bool m_requestStop;

    static GameServer* s_pInstance;
};
