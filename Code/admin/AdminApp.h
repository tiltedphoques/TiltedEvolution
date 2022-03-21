#pragma once

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <AdminMessages/Message.h>
#include <Messages/Message.h>
#include "Overlay.h"

#include <Client.hpp>

using namespace Magnum;
using namespace Math::Literals;

enum class ConnectionState
{
    kNone,
    kConnecting,
    kConnected
};

struct AdminSessionOpen;
struct ServerLogs;

struct AdminApp : Platform::Application, TiltedPhoques::Client
{
    explicit AdminApp(const Arguments& arguments);

    void drawEvent() override;
    void tickEvent() override;

    void viewportEvent(ViewportEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;

    void OnConsume(const void* apData, uint32_t aSize) override;
    void OnConnected() override;
    void OnDisconnected(EDisconnectReason aReason) override;
    void OnUpdate() override;

    void SendShutdownRequest();

protected:

    void drawServerUi();

    bool Send(const ClientAdminMessage& acMessage) const noexcept;
    bool Send(const ClientMessage& acMessage) const noexcept;

    void HandleMessage(const AdminSessionOpen& acMessage);
    void HandleMessage(const ServerLogs& acMessage);

private:
    ImGuiIntegration::Context m_imgui{NoCreate};

    bool m_authenticated;
    Color4 m_clearColor = 0x72909aff_rgbaf;
    Float m_floatValue = 0.0f;
    ConnectionState m_state = ConnectionState::kNone;
    String m_password;
    std::function<void(TiltedPhoques::UniquePtr<ServerAdminMessage>&)> m_messageHandlers[kServerAdminOpcodeMax];
    Overlay m_overlay;
};
