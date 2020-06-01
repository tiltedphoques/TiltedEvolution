#pragma once

#include "Client.hpp"

struct ImguiService;
struct CellChangeEvent;
struct UpdateEvent;

struct World;

namespace TiltedMessages 
{
    class ClientMessage;
}

using TiltedPhoques::Client;

struct TransportService : Client
{
    TransportService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) noexcept;
    ~TransportService() noexcept = default;

    TP_NOCOPYMOVE(TransportService);

    bool Send(const TiltedMessages::ClientMessage& acMessage) const noexcept;

    void OnConsume(const void* apData, uint32_t aSize) override;
    void OnConnected() override;
    void OnDisconnected(EDisconnectReason aReason) override;
    void OnUpdate() override;

    [[nodiscard]] bool IsOnline() const noexcept { return m_connected; }

protected:

    // Event handlers
    void HandleUpdate(const UpdateEvent& acEvent) noexcept;
    void OnCellChangeEvent(const CellChangeEvent& acEvent) const noexcept;
    void OnDraw() noexcept;

    // Packet handlers
    void HandleAuthenticationResponse(const TiltedMessages::AuthenticationResponse& acMessage) noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;
    bool m_connected;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_cellChangeConnection;
    entt::scoped_connection m_drawImGuiConnection;

};
