#include <TiltedOnlinePCH.h>

#include <Services/OverlayService.h>

#include <OverlayApp.hpp>

#include <D3D11Hook.hpp>
#include <OverlayRenderHandlerD3D11.hpp>

#include <Systems/RenderSystemD3D11.h>

#include <World.h>

#include <Services/OverlayClient.h>
#include <Services/TransportService.h>

#include <Messages/NotifyChatMessageBroadcast.h>
#include <Messages/NotifyPlayerList.h>
#include <Messages/NotifyPlayerDialogue.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ConnectionErrorEvent.h>

#include <PlayerCharacter.h>

using TiltedPhoques::OverlayRenderHandlerD3D11;
using TiltedPhoques::OverlayRenderHandler;

struct D3D11RenderProvider final : OverlayApp::RenderProvider, OverlayRenderHandlerD3D11::Renderer
{
    explicit D3D11RenderProvider(RenderSystemD3D11* apRenderSystem) : m_pRenderSystem(apRenderSystem) {}

    OverlayRenderHandler* Create() override
    {
        auto* pHandler = new OverlayRenderHandlerD3D11(this);
        pHandler->SetVisible(true);

        return pHandler;
    }

    [[nodiscard]] HWND GetWindow() override
    {
        return m_pRenderSystem->GetWindow();
    }

    [[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept override
    {
        return m_pRenderSystem->GetSwapChain();
    }

private:

    RenderSystemD3D11* m_pRenderSystem;
};

OverlayService::OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(transport)
{
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&OverlayService::OnConnectedEvent>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&OverlayService::OnDisconnectedEvent>(this);
    m_connectionErrorConnection =
        aDispatcher.sink<ConnectionErrorEvent>().connect<&OverlayService::OnConnectionError>(this);
    //m_playerListConnection = aDispatcher.sink<NotifyPlayerList>().connect<&OverlayService::OnPlayerList>(this);
    //m_cellChangeEventConnection = aDispatcher.sink<CellChangeEvent>().connect<&OverlayService::OnCellChangeEvent>(this);
    m_chatMessageConnection = aDispatcher.sink<NotifyChatMessageBroadcast>().connect<&OverlayService::OnChatMessageReceived>(this);
    m_playerDialogueConnection = aDispatcher.sink<NotifyPlayerDialogue>().connect<&OverlayService::OnPlayerDialogue>(this);
}

OverlayService::~OverlayService() noexcept
{
}

void OverlayService::Create(RenderSystemD3D11* apRenderSystem) noexcept
{
    m_pProvider = TiltedPhoques::MakeUnique<D3D11RenderProvider>(apRenderSystem);
    m_pOverlay = new OverlayApp(m_pProvider.get(), new ::OverlayClient(m_transport, m_pProvider->Create()));

    // NULL PATH FOR NOW
    if (!m_pOverlay->Initialize(""))
        spdlog::error("Overlay could not be initialised");

    m_pOverlay->GetClient()->Create();
}

void OverlayService::Render() noexcept
{
    static bool s_bi = false;
    if (!s_bi)
    {
        m_pOverlay->GetClient()->GetBrowser()->GetHost()->WasResized();

        s_bi = true;
    }

    auto pPlayer = PlayerCharacter::Get();
    bool inGame = pPlayer && pPlayer->GetNiNode();
    if (inGame && !m_inGame)
        SetInGame(true);
    else if (!inGame && m_inGame)
        SetInGame(false);

    m_pOverlay->GetClient()->Render();
}

void OverlayService::Reset() const noexcept
{
    m_pOverlay->GetClient()->Reset();
}

void OverlayService::Initialize() noexcept
{
    m_pOverlay->ExecuteAsync("init");
}

void OverlayService::SetActive(bool aActive) noexcept
{
    if (!m_inGame)
        return;
    if (m_active == aActive)
        return;

    m_active = aActive;

    m_pOverlay->ExecuteAsync(m_active ? "activate" : "deactivate");
}

bool OverlayService::GetActive() const noexcept
{
    return m_active;
}

void OverlayService::SetInGame(bool aInGame) noexcept
{
    if (m_inGame == aInGame)
        return;
    m_inGame = aInGame;

    if (m_inGame)
    {
        SetVersion(BUILD_COMMIT);
        m_pOverlay->ExecuteAsync("entergame");
    }
    else
    {
        m_pOverlay->ExecuteAsync("exitgame");
        SetActive(false);
    }
}

bool OverlayService::GetInGame() const noexcept
{
    return m_inGame;
}

void OverlayService::SetVersion(const std::string& acVersion)
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();

    pArguments->SetString(0, acVersion);
    m_pOverlay->ExecuteAsync("versionset", pArguments);
}

void OverlayService::SendSystemMessage(const std::string& acMessage)
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetString(0, acMessage);
    m_pOverlay->ExecuteAsync("systemmessage", pArguments);
}

void OverlayService::OnChatMessageReceived(const NotifyChatMessageBroadcast& acMessage) noexcept
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetString(0, acMessage.PlayerName.c_str());
    pArguments->SetString(1, acMessage.ChatMessage.c_str());
    spdlog::debug("Received Message from Server and gonna send it to UI: " + acMessage.ChatMessage);
    m_pOverlay->ExecuteAsync("message", pArguments);
}

void OverlayService::OnConnectedEvent(const ConnectedEvent&) noexcept
{
    m_pOverlay->ExecuteAsync("connect");
    SendSystemMessage("Successfully connected to server");
}

void OverlayService::OnDisconnectedEvent(const DisconnectedEvent&) noexcept
{
    m_pOverlay->ExecuteAsync("disconnect");
    SendSystemMessage("Disconnected from server");
}

void OverlayService::OnPlayerDialogue(const NotifyPlayerDialogue& acMessage) noexcept
{
    SendSystemMessage(acMessage.Text.c_str());
}

void OverlayService::OnConnectionError(const ConnectionErrorEvent& acConnectedEvent) const noexcept
{
    auto pArgs = CefListValue::Create();
    pArgs->SetString(0, acConnectedEvent.ErrorDetail.c_str());
    m_pOverlay->ExecuteAsync("triggererror", pArgs);
}

void OverlayService::OnPlayerList(const NotifyPlayerList& acPlayerList) noexcept
{
    for (auto& player : acPlayerList.Players)
    {
        spdlog::info("[CLIENT] ID: {} - Name: {}", player.first, player.second);

        /*
        auto pArguments = CefListValue::Create();
        pArguments->SetInt(0, player.first);
        pArguments->SetString(1, player.second.c_str());
        pArguments->SetInt(2, 7);
        pArguments->SetString(3, "House");
        m_pOverlay->ExecuteAsync("playerconnected", pArguments);
        */
    }
}

#if 0
void OverlayService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, SERVERID);
    pArguments->SetString(1, USERNAME);
    m_pOverlay->ExecuteAsync("playerdisconnected");
}

void OverlayService::OnCellChangeEvent(const CellChangeEvent& aCellChangeEvent) noexcept
{
    spdlog::warn("OnCellChangeEvent ! %s", aCellChangeEvent.Name);
    // Hacky as fuck... Idk why the first cellchangeevent broke UI. It's not a big deal because when player connected we
    // force cell changed event But change event come before connected event.
    static bool firstCellChangeEvent = false;
    if (!m_pOverlay || !firstCellChangeEvent)
    {
        firstCellChangeEvent = true;
        return;
    }
    SendSystemMessage("On Cell change event");
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, 1);
    pArguments->SetString(1, aCellChangeEvent.Name);
    m_pOverlay->ExecuteAsync("setcell", pArguments);
    spdlog::warn("OnCellChangeEvent end !");
}
#endif
