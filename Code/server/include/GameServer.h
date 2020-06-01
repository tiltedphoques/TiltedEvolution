#pragma once

#include <World.h>

using TiltedPhoques::String;
using TiltedPhoques::Server;
using TiltedPhoques::ConnectionId_t;

struct GameServer final : Server
{
    GameServer(uint16_t aPort, bool aPremium, String aName, String aToken) noexcept;
    virtual ~GameServer();

    TP_NOCOPYMOVE(GameServer);

    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId_t aConnectionId) override;
    void OnConnection(ConnectionId_t aHandle) override;
    void OnDisconnection(ConnectionId_t aConnectionId) override;

    void Send(ConnectionId_t aConnectionId, const TiltedMessages::ServerMessage& acServerMessage) const;
    void SendToLoaded(const TiltedMessages::ServerMessage& acServerMessage) const;

    static GameServer* Get() noexcept;

protected:

    void HandleAuthenticationRequest(ConnectionId_t aConnectionId, const TiltedMessages::AuthenticationRequest& acRequest) noexcept;

private:

    void SetTitle() const;

    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    String m_name;
    String m_token;

    World m_world;
    static GameServer* s_pInstance;
};
