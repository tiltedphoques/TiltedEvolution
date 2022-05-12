#pragma once

#include <include/internal/cef_ptr.h>

namespace TiltedPhoques
{
struct OverlayApp;
}

struct RenderSystemD3D11;
struct D3D11RenderProvider;
struct FormIdComponent;
struct World;
struct UpdateEvent;
struct ConnectedEvent;
struct DisconnectedEvent;
struct TransportService;
struct NotifyChatMessageBroadcast;
struct NotifyPlayerList;
struct NotifyPlayerJoined;
struct NotifyPlayerLeft;
struct NotifyPlayerDialogue;
struct ConnectionErrorEvent;
struct RemotePlayerSpawnedEvent;
struct NotifyPlayerLevel;
struct NotifyHealthChangeBroadcast;
struct NotifyPlayerCellChanged;


using TiltedPhoques::OverlayApp;

/**
* @brief Controls the UI from the client.
*/
struct OverlayService
{
    OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher);
    ~OverlayService() noexcept;

    TP_NOCOPYMOVE(OverlayService);

    void Create(RenderSystemD3D11* apRenderSystem) noexcept;

    void Render() noexcept;
    void Reset() const noexcept;

    void Initialize() noexcept;

    void SetActive(bool aActive) noexcept;
    [[nodiscard]] bool GetActive() const noexcept;

    void SetInGame(bool aInGame) noexcept;
    [[nodiscard]] bool GetInGame() const noexcept;

    void SetVersion(const std::string& acVersion);

    OverlayApp* GetOverlayApp() const noexcept
    {
        return m_pOverlay.get();
    }

    void SendSystemMessage(const std::string& acMessage);

  protected:
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnConnectedEvent(const ConnectedEvent&) noexcept;
    void OnDisconnectedEvent(const DisconnectedEvent&) noexcept;
    void OnConnectionError(const ConnectionErrorEvent& acConnectedEvent) const noexcept;
    void OnChatMessageReceived(const NotifyChatMessageBroadcast&) noexcept;
    void OnPlayerDialogue(const NotifyPlayerDialogue&) noexcept;
    void OnPlayerJoined(const NotifyPlayerJoined&) noexcept;
    void OnPlayerLeft(const NotifyPlayerLeft&) noexcept;
    void OnRemotePlayerSpawned(const RemotePlayerSpawnedEvent&) noexcept;
    void OnPlayerLevel(const NotifyPlayerLevel&) noexcept;
    void OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acMessage) const noexcept;
    void OnPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept;

  private:
    CefRefPtr<OverlayApp> m_pOverlay{nullptr};
    TiltedPhoques::UniquePtr<D3D11RenderProvider> m_pProvider;

    World& m_world;
    TransportService& m_transport;

    bool m_active = false;
    bool m_inGame = false;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_connectionErrorConnection;
    entt::scoped_connection m_cellChangeEventConnection;
    entt::scoped_connection m_chatMessageConnection;
    entt::scoped_connection m_playerDialogueConnection;
    entt::scoped_connection m_playerJoinedConnection;
    entt::scoped_connection m_playerLeftConnection;
    entt::scoped_connection m_remotePlayerSpawnedConnection;
    entt::scoped_connection m_playerLevelConnection;
    entt::scoped_connection m_healthChangeConnection;
    entt::scoped_connection m_cellChangedConnection;
};
