#pragma once

#include <AdminMessages/Message.h>
#include <Messages/AuthenticationRequest.h>
#include <Messages/Message.h>
#include <World.h>

using TiltedPhoques::ConnectionId_t;
using TiltedPhoques::Server;
using TiltedPhoques::String;

struct AuthenticationRequest;
struct Player;
struct PartyComponent;

namespace Resources
{
struct ResourceCollection;
}

namespace Console
{
class ConsoleRegistry;
}

struct GameServer final : Server
{
    // TODO: eventually refactor this.
    struct Info
    {
        String name;
        String desc;
        String icon_url;
        String tagList;
        uint16_t tick_rate;
    };

    GameServer(Console::ConsoleRegistry& aConsole) noexcept;
    virtual ~GameServer();

    TP_NOCOPYMOVE(GameServer);

    static GameServer* Get() noexcept;

    void Initialize();
    void Kill();

    bool CheckMoPo();
    void BindMessageHandlers();
    void BindServerCommands();

    void UpdateInfo();
    void UpdateTimeScale();
    void UpdateSettings();

    // Packet dispatching
    void Send(ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const;
    void Send(ConnectionId_t aConnectionId, const ServerAdminMessage& acServerMessage) const;
    void SendToLoaded(const ServerMessage& acServerMessage) const;
    void SendToPlayers(const ServerMessage& acServerMessage, const Player* apExcludeSender = nullptr) const;
    bool SendToPlayersInRange(const ServerMessage& acServerMessage, const entt::entity acOrigin,
                              const Player* apExcludeSender = nullptr) const;
    void SendToParty(const ServerMessage& acServerMessage, const PartyComponent& acPartyComponent,
                     const Player* apExcludeSender = nullptr) const;
    void SendToPartyInRange(const ServerMessage& acServerMessage, const PartyComponent& acPartyComponent,
                            const entt::entity acOrigin, const Player* apExcludeSender = nullptr) const;

    const Info& GetInfo() const noexcept
    {
        return m_info;
    }

    bool IsRunning() const noexcept
    {
        return !m_requestStop;
    }
    bool IsPasswordProtected() const noexcept
    {
        return m_isPasswordProtected;
    }
    [[nodiscard]] bool IsPublic() const noexcept
    {
        return m_isPublic;
    }

    template <class T> void ForEachAdmin(const T& aFunctor)
    {
        for (auto id : m_adminSessions)
            aFunctor(id);
    }

    struct Uptime
    {
        int weeks;
        int days;
        int hours;
        int minutes;
    };
    Uptime GetUptime() const noexcept;

    World& GetWorld() const noexcept
    {
        return *m_pWorld;
    }

    [[nodiscard]] const TiltedPhoques::Set<ConnectionId_t>& GetAdminSessions() const noexcept
    {
        return m_adminSessions;
    }

    void AddAdminSession(ConnectionId_t acSession) noexcept
    {
        m_adminSessions.insert(acSession);
    }

    void RemoveAdminSession(ConnectionId_t acSession) noexcept
    {
        m_adminSessions.erase(acSession);
    }

    Player* GetAdminByUsername(const String& acUsername)  const noexcept;
    Player const* GetAdminByUsername(const String& acUsername) noexcept;

  protected:
    bool ValidateAuthParams(ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest);
    void HandleAuthenticationRequest(ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest);

    // Implement TiltedPhoques::Server
    void OnUpdate() override;
    void OnConsume(const void* apData, uint32_t aSize, ConnectionId_t aConnectionId) override;
    void OnConnection(ConnectionId_t aHandle) override;
    void OnDisconnection(ConnectionId_t aConnectionId, EDisconnectReason aReason) override;

  private:
    void UpdateTitle() const;
    String SanitizeUsername(const String& acUsername) const noexcept;

  private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::function<void(UniquePtr<ClientMessage>&, ConnectionId_t)> m_messageHandlers[kClientOpcodeMax];
    std::function<void(UniquePtr<ClientAdminMessage>&, ConnectionId_t)> m_adminMessageHandlers[kClientAdminOpcodeMax];

    bool m_isPasswordProtected{};
    bool m_isPublic{};

    Info m_info{};
    UniquePtr<Resources::ResourceCollection> m_pResources;
    Console::ConsoleRegistry& m_commands;

    TiltedPhoques::Set<ConnectionId_t> m_adminSessions;
    TiltedPhoques::Map<ConnectionId_t, entt::entity> m_connectionToEntity;

    UniquePtr<World> m_pWorld;

    bool m_requestStop;

    static inline GameServer* s_pInstance = nullptr;
};
