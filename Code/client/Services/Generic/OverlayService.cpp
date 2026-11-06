#include <TiltedOnlinePCH.h>

#include <Services/OverlayService.h>

#include <OverlayApp.hpp>

#include <D3D11Hook.hpp>
#include <OverlayRenderHandlerD3D11.hpp>

#include <algorithm>
#include <ws2tcpip.h>

#include <base/threading/ThreadUtils.h>

#include <Systems/RenderSystemD3D11.h>

#include <World.h>

#include <Services/OverlayClient.h>
#include <Services/TransportService.h>

#include <Messages/NotifyChatMessageBroadcast.h>
#include <Messages/NotifyPlayerList.h>
#include <Messages/NotifyPlayerLeft.h>
#include <Messages/NotifyPlayerJoined.h>
#include <Messages/NotifyPlayerDialogue.h>
#include <Messages/NotifyPlayerLevel.h>
#include <Messages/NotifyPlayerCellChanged.h>
#include <Messages/NotifyTeleport.h>
#include <Messages/RequestPlayerHealthUpdate.h>
#include <Messages/NotifyPlayerHealthUpdate.h>

#include <Structs/GridCellCoords.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ConnectionErrorEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>

#include <PlayerCharacter.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>
#include <Games/ActorExtension.h>

using TiltedPhoques::OverlayRenderHandler;
using TiltedPhoques::OverlayRenderHandlerD3D11;

struct D3D11RenderProvider final : OverlayApp::RenderProvider, OverlayRenderHandlerD3D11::Renderer
{
    explicit D3D11RenderProvider(RenderSystemD3D11* apRenderSystem)
        : m_pRenderSystem(apRenderSystem)
    {
    }

    OverlayRenderHandler* Create() override
    {
        auto* pHandler = new OverlayRenderHandlerD3D11(this);
        pHandler->SetVisible(true);

        return pHandler;
    }

    [[nodiscard]] HWND GetWindow() override { return m_pRenderSystem->GetWindow(); }

    [[nodiscard]] IDXGISwapChain* GetSwapChain() const noexcept override { return m_pRenderSystem->GetSwapChain(); }
    [[nodiscard]] ID3D11Device* GetDevice() const noexcept override { return m_pRenderSystem->GetDevice(); }
    [[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const noexcept override { return m_pRenderSystem->GetDeviceContext(); }

private:
    RenderSystemD3D11* m_pRenderSystem;
};

/**
 * @brief Makes CEF draw the UI on the CPU instead of the GPU.
 *
 * On the GPU, CEF copies every frame back to memory for OnPaint, and that waits behind the game's frames when
 * the game keeps the GPU busy: CEF drops UI frames and the UI shows up half drawn.
 */
struct CpuRenderedOverlayApp final : OverlayApp
{
    using OverlayApp::OverlayApp;

    void OnBeforeCommandLineProcessing(const CefString& acProcessType, CefRefPtr<CefCommandLine> apCommandLine) override
    {
        OverlayApp::OnBeforeCommandLineProcessing(acProcessType, apCommandLine);

        apCommandLine->AppendSwitch("disable-gpu");
        apCommandLine->AppendSwitch("disable-gpu-compositing");

        spdlog::info("Overlay: CEF draws the UI on the CPU (disable-gpu, disable-gpu-compositing)");
    }
};

constexpr uint8_t kChallengeRequestMsgId = 32;
constexpr uint8_t kChallengeReplyMsgId = 33;
constexpr uint8_t kConnectionIdKey = 0x0D;
constexpr uint8_t kChallengeKey = 0x11;
constexpr uint8_t kTimestampKey = 0x19;
constexpr uint8_t kProtocolVersionKey = 0x20;
constexpr uint8_t kProtocolVersion = 11;
constexpr uint64_t kPingSendIntervalUs = 10'000;

#pragma pack(push, 1)
struct UdpChallengeRequest
{
    uint8_t MsgId = kChallengeRequestMsgId;
    uint16_t MsgLength = 16;
    uint8_t ConnectionIdKey = kConnectionIdKey;
    uint32_t ConnectionId;
    uint8_t MyTimestampKey = kTimestampKey;
    uint64_t MyTimestamp;
    uint8_t ProtocolVersionKey = kProtocolVersionKey;
    uint8_t ProtocolVersion = kProtocolVersion;
    uint8_t Padding[493]{};
};

struct UdpChallengeReply
{
    uint8_t MsgId;
    uint8_t ConnectionIdKey;
    uint32_t ConnectionId;
    uint8_t ChallengeKey;
    uint64_t Challenge;
    uint8_t YourTimestampKey;
    uint64_t YourTimestamp;
};
#pragma pack(pop)

static_assert(sizeof(UdpChallengeRequest) == 512);
static_assert(offsetof(UdpChallengeRequest, Padding) - offsetof(UdpChallengeRequest, ConnectionIdKey) == UdpChallengeRequest{}.MsgLength);

static uint64_t GetPingTime() noexcept
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

String GetCellName(const GameId& aWorldSpaceId, const GameId& aCellId) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    String cellName = "UNKNOWN";

    if (aWorldSpaceId)
    {
        const uint32_t worldSpaceId = modSystem.GetGameId(aWorldSpaceId);
        TESWorldSpace* pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(worldSpaceId));
        if (pWorldSpace)
            cellName = pWorldSpace->GetName();
    }
    else
    {
        const uint32_t cellId = modSystem.GetGameId(aCellId);
        TESObjectCELL* pCell = Cast<TESObjectCELL>(TESForm::GetById(cellId));
        if (pCell)
            cellName = pCell->GetName();
    }

    return cellName;
}

float CalculateHealthPercentage(Actor* apActor) noexcept
{
    const float maxHealth = apActor->GetActorPermanentValue(ActorValueInfo::kHealth);
    const float tempModHealth = apActor->healthModifiers.temporaryModifier;

    if (maxHealth == 0.f)
        return 0.f;
    const float health = apActor->GetActorValue(ActorValueInfo::kHealth);

    float percentage = health / (maxHealth + tempModHealth) * 100.f;
    if (percentage < 0.f)
        percentage = 0.f;

    return percentage;
}

OverlayService::OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
    , m_transport(transport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&OverlayService::OnUpdate>(this);
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&OverlayService::OnConnectedEvent>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&OverlayService::OnDisconnectedEvent>(this);
    m_connectionErrorConnection = aDispatcher.sink<ConnectionErrorEvent>().connect<&OverlayService::OnConnectionError>(this);
    m_chatMessageConnection = aDispatcher.sink<NotifyChatMessageBroadcast>().connect<&OverlayService::OnChatMessageReceived>(this);
    m_playerJoinedConnection = aDispatcher.sink<NotifyPlayerJoined>().connect<&OverlayService::OnPlayerJoined>(this);
    m_playerLeftConnection = aDispatcher.sink<NotifyPlayerLeft>().connect<&OverlayService::OnPlayerLeft>(this);
    m_playerDialogueConnection = aDispatcher.sink<NotifyPlayerDialogue>().connect<&OverlayService::OnPlayerDialogue>(this);
    m_playerAddedConnection = m_world.on_destroy<WaitingFor3D>().connect<&OverlayService::OnWaitingFor3DRemoved>(this);
    m_playerRemovedConnection = m_world.on_destroy<PlayerComponent>().connect<&OverlayService::OnPlayerComponentRemoved>(this);
    m_playerLevelConnection = aDispatcher.sink<NotifyPlayerLevel>().connect<&OverlayService::OnPlayerLevel>(this);
    m_cellChangedConnection = aDispatcher.sink<NotifyPlayerCellChanged>().connect<&OverlayService::OnPlayerCellChanged>(this);
    m_teleportConnection = aDispatcher.sink<NotifyTeleport>().connect<&OverlayService::OnNotifyTeleport>(this);
    m_playerHealthConnection = aDispatcher.sink<NotifyPlayerHealthUpdate>().connect<&OverlayService::OnNotifyPlayerHealthUpdate>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&OverlayService::OnPartyJoinedEvent>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&OverlayService::OnPartyLeftEvent>(this);
}

OverlayService::~OverlayService() noexcept
{
    ClosePingSocket();
}

void OverlayService::Create(RenderSystemD3D11* apRenderSystem) noexcept
{
    m_pProvider = TiltedPhoques::MakeUnique<D3D11RenderProvider>(apRenderSystem);
    m_pOverlay = new CpuRenderedOverlayApp(m_pProvider.get(), new ::OverlayClient(m_transport, m_pProvider->Create()));

    if (!m_pOverlay->Initialize())
    {
        spdlog::error("Overlay could not be initialized");
        if (int32_t exitCode = CefGetExitCode())
        {
            spdlog::critical("CEF failed to initialize, exit code {}. See 'cef_types.h' for description", exitCode);
        }
    }

    m_pOverlay->GetClient()->Create();
}

void OverlayService::Render() noexcept
{
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

void OverlayService::Reload() noexcept
{
    SetInGame(false);
    SetActive(false);
    GetOverlayApp()->GetClient()->GetBrowser()->Reload();
    Initialize();
    SetInGame(true);
    m_pOverlay->ExecuteAsync("enterGame");
    SetActive(true);
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
        m_pOverlay->ExecuteAsync("enterGame");
    }
    else
    {
        m_pOverlay->ExecuteAsync("exitGame");
        // TODO: this does nothing, since m_inGame is false
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
    m_pOverlay->ExecuteAsync("setVersion", pArguments);
}

void OverlayService::SendSystemMessage(const std::string& acMessage)
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, kSystemMessage);
    pArguments->SetString(1, acMessage);

    m_pOverlay->ExecuteAsync("message", pArguments);
}

void OverlayService::SetPlayerHealthPercentage(uint32_t aFormId) const noexcept
{
    Actor* pActor = Cast<Actor>(TESForm::GetById(aFormId));
    if (!pActor)
    {
        spdlog::error("{}: cannot find actor for form id {:X}", __FUNCTION__, aFormId);
        return;
    }

    float percentage = CalculateHealthPercentage(pActor);

    auto view = m_world.view<FormIdComponent, PlayerComponent>();
    auto entityIt = std::find_if(view.begin(), view.end(), [view, aFormId](auto aEntity) { return view.get<FormIdComponent>(aEntity).Id == aFormId; });

    if (entityIt == view.end())
    {
        spdlog::error("{}: cannot find player entity for form id {:X}", __FUNCTION__, aFormId);
        return;
    }

    const auto& playerComponent = view.get<PlayerComponent>(*entityIt);

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, playerComponent.Id);
    pArguments->SetDouble(1, static_cast<double>(percentage));
    m_pOverlay->ExecuteAsync("setHealth", pArguments);
}

void OverlayService::PingServers(const ServerAddresses& acServers) noexcept
{
    ClosePingSocket();

    m_pendingPings = acServers;
    std::ranges::reverse(m_pendingPings);

    DWORD v6Only = 0;
    sockaddr_in6 local{};
    local.sin6_family = AF_INET6;

    m_pingSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (m_pingSocket == INVALID_SOCKET || setsockopt(m_pingSocket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6Only), sizeof(v6Only)) == SOCKET_ERROR ||
        bind(m_pingSocket, reinterpret_cast<const sockaddr*>(&local), sizeof(local)) == SOCKET_ERROR)
    {
        spdlog::warn("Server ping socket setup failed: {}", WSAGetLastError());
        ClosePingSocket();
        return;
    }

    m_pingThread = CreateThread(
        nullptr, 0,
        [](LPVOID apThis) -> DWORD
        {
            static_cast<OverlayService*>(apThis)->ReceiveServerPings();
            return 0;
        },
        this, 0, nullptr);
    if (!m_pingThread)
    {
        spdlog::warn("Server ping thread creation failed: {}", GetLastError());
        ClosePingSocket();
    }
}

void OverlayService::PingServer(uint32_t aId, const std::string& acIp, uint16_t aPort) noexcept
{
    SteamNetworkingIPAddr ip{};
    if (!ip.ParseString(acIp.c_str()))
        return;

    sockaddr_in6 address{};
    address.sin6_family = AF_INET6;
    address.sin6_port = htons(aPort);
    std::memcpy(&address.sin6_addr, ip.m_ipv6, sizeof(ip.m_ipv6));

    UdpChallengeRequest request{};
    request.ConnectionId = aId;
    request.MyTimestamp = GetPingTime();

    sendto(m_pingSocket, reinterpret_cast<const char*>(&request), sizeof(request), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
}

void OverlayService::OnUpdate(const UpdateEvent&) noexcept
{
    RunDebugDataUpdates();
    RunPlayerHealthUpdates();
    RunServerPingUpdates();
}

void OverlayService::OnConnectedEvent(const ConnectedEvent& acEvent) noexcept
{
    m_pOverlay->ExecuteAsync("connect");

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acEvent.PlayerId);
    m_pOverlay->ExecuteAsync("setLocalPlayerId", pArguments);
}

void OverlayService::OnDisconnectedEvent(const DisconnectedEvent&) noexcept
{
    m_pOverlay->ExecuteAsync("disconnect");
}

void OverlayService::OnWaitingFor3DRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept
{
    const auto* pPlayerComponent = m_world.try_get<PlayerComponent>(aEntity);
    if (!pPlayerComponent)
        return;

    const auto& formIdComponent = m_world.get<FormIdComponent>(aEntity);

    Actor* pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
    if (!pActor)
    {
        spdlog::error("{}: cannot find actor for form id {:X}", __FUNCTION__, formIdComponent.Id);
        return;
    }

    float percentage = CalculateHealthPercentage(pActor);

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, pPlayerComponent->Id);
    pArguments->SetInt(1, static_cast<int>(percentage));

    m_pOverlay->ExecuteAsync("setPlayer3dLoaded", pArguments);
}

void OverlayService::OnPlayerComponentRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept
{
    const auto& playerComponent = m_world.get<PlayerComponent>(aEntity);

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, playerComponent.Id);

    m_pOverlay->ExecuteAsync("setPlayer3dUnloaded", pArguments);
}

void OverlayService::OnChatMessageReceived(const NotifyChatMessageBroadcast& acMessage) noexcept
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, (int)acMessage.MessageType);
    pArguments->SetString(1, acMessage.ChatMessage.c_str());
    pArguments->SetString(2, acMessage.PlayerName.c_str());

    m_pOverlay->ExecuteAsync("message", pArguments);
}

void OverlayService::OnPlayerDialogue(const NotifyPlayerDialogue& acMessage) noexcept
{
    if (!m_pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, kPlayerDialogue);
    pArguments->SetString(1, acMessage.Text.c_str());
    pArguments->SetString(2, acMessage.Name.c_str());

    m_pOverlay->ExecuteAsync("message", pArguments);
}

void OverlayService::OnConnectionError(const ConnectionErrorEvent& acConnectedEvent) const noexcept
{
    auto pArgs = CefListValue::Create();
    pArgs->SetString(0, acConnectedEvent.ErrorDetail.c_str());
    m_pOverlay->ExecuteAsync("triggerError", pArgs);
}

void OverlayService::OnPlayerJoined(const NotifyPlayerJoined& acMessage) noexcept
{
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acMessage.PlayerId);
    pArguments->SetString(1, acMessage.Username.c_str());
    pArguments->SetInt(2, acMessage.Level);

    String cellName = GetCellName(acMessage.WorldSpaceId, acMessage.CellId);
    pArguments->SetString(3, cellName.c_str());

    m_pOverlay->ExecuteAsync("playerConnected", pArguments);
}

void OverlayService::OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept
{
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acMessage.PlayerId);
    pArguments->SetString(1, acMessage.Username.c_str());
    m_pOverlay->ExecuteAsync("playerDisconnected", pArguments);
}

void OverlayService::OnPlayerLevel(const NotifyPlayerLevel& acMessage) noexcept
{
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acMessage.PlayerId);
    pArguments->SetInt(1, acMessage.NewLevel);
    m_pOverlay->ExecuteAsync("setLevel", pArguments);
}

void OverlayService::OnPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept
{
    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acMessage.PlayerId);
    String cellName = GetCellName(acMessage.WorldSpaceId, acMessage.CellId);
    pArguments->SetString(1, cellName.c_str());
    m_pOverlay->ExecuteAsync("setCell", pArguments);
}

void OverlayService::OnNotifyTeleport(const NotifyTeleport& acMessage) noexcept
{
    auto& modSystem = m_world.GetModSystem();

    const uint32_t cellId = modSystem.GetGameId(acMessage.CellId);
    TESObjectCELL* pCell = Cast<TESObjectCELL>(TESForm::GetById(cellId));
    if (!pCell)
    {
        const uint32_t worldSpaceId = modSystem.GetGameId(acMessage.WorldSpaceId);
        TESWorldSpace* pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(worldSpaceId));
        if (pWorldSpace)
        {
            GridCellCoords coordinates = GridCellCoords::CalculateGridCellCoords(acMessage.Position);
            pCell = pWorldSpace->LoadCell(coordinates.X, coordinates.Y);
        }

        if (!pCell)
        {
            spdlog::error("Failed to fetch cell to teleport to.");
            m_world.GetOverlayService().SendSystemMessage("Teleporting to player failed.");
            return;
        }
    }

    PlayerCharacter::Get()->MoveTo(pCell, acMessage.Position);
}

void OverlayService::OnNotifyPlayerHealthUpdate(const NotifyPlayerHealthUpdate& acMessage) noexcept
{
    const float percentage = acMessage.Percentage >= 0.f ? acMessage.Percentage : 0.f;

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, acMessage.PlayerId);
    pArguments->SetDouble(1, static_cast<double>(percentage));
    m_pOverlay->ExecuteAsync("setHealth", pArguments);
}

void OverlayService::OnPartyJoinedEvent(const PartyJoinedEvent& acEvent) noexcept
{
    if (acEvent.IsLeader)
        m_world.GetOverlayService().GetOverlayApp()->ExecuteAsync("partyCreated");
}

void OverlayService::OnPartyLeftEvent(const PartyLeftEvent& acEvent) noexcept
{
    m_world.GetOverlayService().GetOverlayApp()->ExecuteAsync("partyLeft");
}

void OverlayService::RunDebugDataUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 1000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    auto internalStats = m_transport.GetStatistics();
    auto steamStats = m_transport.GetConnectionStatus();

    auto pArguments = CefListValue::Create();
    pArguments->SetInt(0, steamStats.m_flOutPacketsPerSec);
    pArguments->SetInt(1, steamStats.m_flInPacketsPerSec);
    pArguments->SetInt(2, steamStats.m_nPing);
    pArguments->SetInt(3, 0);
    pArguments->SetInt(4, internalStats.SentBytes);
    pArguments->SetInt(5, internalStats.RecvBytes);

    m_pOverlay->ExecuteAsync("debugData", pArguments);
}

// TODO(cosideci): this whole thing is a really hacky solution to
// health sync code being somewhat broken for players.
void OverlayService::RunPlayerHealthUpdates() noexcept
{
    if (!m_transport.IsConnected() || !m_world.GetPartyService().IsInParty())
        return;

    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 500ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    static float s_previousPercentage = -1.f;

    const float newPercentage = CalculateHealthPercentage(PlayerCharacter::Get());
    if (newPercentage == s_previousPercentage)
        return;

    s_previousPercentage = newPercentage;

    RequestPlayerHealthUpdate request{};
    request.Percentage = newPercentage;

    m_transport.Send(request);
}

void OverlayService::RunServerPingUpdates() noexcept
{
    if (m_pingSocket == INVALID_SOCKET)
        return;

    const uint64_t cNow = GetPingTime();
    if (!m_pendingPings.empty() && cNow - m_lastPingSendTime >= kPingSendIntervalUs)
    {
        const auto [cId, cIp, cPort] = std::move(m_pendingPings.back());
        m_pendingPings.pop_back();
        PingServer(cId, cIp, cPort);
        m_lastPingSendTime = cNow;
    }

    std::scoped_lock lock(m_pingMutex);
    for (const auto& [id, ping] : m_serverPings)
    {
        auto pArguments = CefListValue::Create();
        pArguments->SetInt(0, id);
        pArguments->SetInt(1, ping);
        m_pOverlay->ExecuteAsync("serverPing", pArguments);
    }
    m_serverPings.clear();
}

void OverlayService::ClosePingSocket() noexcept
{
    if (m_pingSocket != INVALID_SOCKET)
        closesocket(m_pingSocket);

    if (m_pingThread)
    {
        WaitForSingleObject(m_pingThread, INFINITE);
        CloseHandle(m_pingThread);
        m_pingThread = nullptr;
    }

    m_pingSocket = INVALID_SOCKET;
}

void OverlayService::ReceiveServerPings() noexcept
{
    Base::SetCurrentThreadName("ServerPings");

    while (true)
    {
        uint8_t buffer[64];
        const int cSize = recv(m_pingSocket, reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
        const uint64_t cNow = GetPingTime();
        if (cSize == SOCKET_ERROR)
        {
            const int cError = WSAGetLastError();
            if (cError == WSAECONNRESET || cError == WSAENETRESET || cError == WSAEMSGSIZE)
                continue;

            if (cError != WSAEINTR && cError != WSAENOTSOCK)
                spdlog::warn("Server ping receive failed: {}", cError);

            return;
        }

        UdpChallengeReply reply;
        if (cSize < static_cast<int>(sizeof(reply)))
            continue;

        std::memcpy(&reply, buffer, sizeof(reply));
        if (reply.MsgId != kChallengeReplyMsgId || reply.ConnectionIdKey != kConnectionIdKey || reply.ChallengeKey != kChallengeKey || reply.YourTimestampKey != kTimestampKey)
            continue;

        if (reply.YourTimestamp > cNow)
            continue;

        std::scoped_lock lock(m_pingMutex);
        m_serverPings.emplace_back(reply.ConnectionId, static_cast<int>((cNow - reply.YourTimestamp) / 1000));
    }
}
