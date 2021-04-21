#include <stdafx.h>
#include <GameServer.h>
#include <Components.h>
#include <Packet.hpp>

#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>

#include <Messages/ClientMessageFactory.h>
#include <Messages/AuthenticationResponse.h>
#include <Scripts/Player.h>

#if TP_PLATFORM_WINDOWS
#include <windows.h>
#endif

GameServer* GameServer::s_pInstance = nullptr;

GameServer::GameServer(uint16_t aPort, bool aPremium, String aName, String aToken) noexcept
    : m_lastFrameTime(std::chrono::high_resolution_clock::now())
    , m_name(std::move(aName)), m_token(std::move(aToken)),
      m_requestStop(false)
{
    assert(s_pInstance == nullptr);

    s_pInstance = this;

    while (!Host(aPort, aPremium ? 60 : 20))
    {
        spdlog::warn("Port {} is already in use, trying {}", aPort, aPort + 1);
        aPort++;
    }

    spdlog::info("Server started on port {}", GetPort());
    SetTitle();

    m_pWorld = std::make_unique<World>();

    auto handlerGenerator = [this](auto& x)
    {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_messageHandlers[T::Opcode] = [this](UniquePtr<ClientMessage>& apMessage, ConnectionId_t aConnectionId) {
            const auto pRealMessage = CastUnique<T>(std::move(apMessage));
            m_pWorld->GetDispatcher().trigger(PacketEvent<T>(pRealMessage.get(), aConnectionId));
        };

        return false;
    };

    ClientMessageFactory::Visit(handlerGenerator);

    // Override authentication request
    m_messageHandlers[AuthenticationRequest::Opcode] = [this](UniquePtr<ClientMessage>& apMessage, ConnectionId_t aConnectionId) {
        const auto pRealMessage = CastUnique<AuthenticationRequest>(std::move(apMessage));
        HandleAuthenticationRequest(aConnectionId, pRealMessage);
    };
}

GameServer::~GameServer()
{
    s_pInstance = nullptr;
}

void GameServer::Initialize()
{
    m_pWorld->GetScriptService().Initialize();
}

void GameServer::OnUpdate()
{
    const auto cNow = std::chrono::high_resolution_clock::now();
    const auto cDelta = cNow - m_lastFrameTime;
    m_lastFrameTime = cNow;

    const auto cDeltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(cDelta).count();

    auto& dispatcher = m_pWorld->GetDispatcher();

    dispatcher.trigger(UpdateEvent{cDeltaSeconds});

    if (m_requestStop)
        Close();
}

void GameServer::OnConsume(const void* apData, const uint32_t aSize, const ConnectionId_t aConnectionId)
{
    ClientMessageFactory factory;
    ViewBuffer buf((uint8_t*)apData, aSize);
    Buffer::Reader reader(&buf);

    auto pMessage = factory.Extract(reader);
    if(!pMessage)
    {
        spdlog::error("Couldn't parse packet from {:x}", aConnectionId);
        return;
    }

    m_messageHandlers[pMessage->GetOpcode()](pMessage, aConnectionId);
}

void GameServer::OnConnection(const ConnectionId_t aHandle)
{
    spdlog::info("Connection received {:x}", aHandle);

    SetTitle();
}

void GameServer::OnDisconnection(const ConnectionId_t aConnectionId, EDisconnectReason aReason)
{
    StackAllocator<1 << 14> allocator;
    ScopedAllocator _{ allocator };

    spdlog::info("Connection ended {:x}", aConnectionId);

    m_pWorld->GetScriptService().HandlePlayerQuit(aConnectionId, aReason);

    Vector<entt::entity> entitiesToDestroy;
    entitiesToDestroy.reserve(500);

    // Find if a player is associated with this connection and delete it
    auto playerView = m_pWorld->view<PlayerComponent>();
    for (auto entity : playerView)
    {
        const auto& [playerComponent] = playerView.get(entity);
        if (playerComponent.ConnectionId == aConnectionId)
        {
            m_pWorld->GetDispatcher().trigger(PlayerLeaveEvent(entity));

            entitiesToDestroy.push_back(entity);
            break;
        }
    }

    // Cleanup all entities that we own
    auto ownerView = m_pWorld->view<OwnerComponent>();
    for (auto entity : ownerView)
    {
        const auto& [ownerComponent] = ownerView.get(entity);
        if (ownerComponent.ConnectionId == aConnectionId)
        {
            entitiesToDestroy.push_back(entity);
        }
    }

    for(auto entity : entitiesToDestroy)
    {
        m_pWorld->remove_if_exists<ScriptsComponent>(entity);
        m_pWorld->destroy(entity);
    }

    SetTitle();
}

void GameServer::Send(const ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const
{
    static thread_local TiltedPhoques::ScratchAllocator s_allocator{ 1 << 18 };

    TiltedPhoques::ScopedAllocator _(s_allocator);

    Buffer buffer(1 << 16);
    Buffer::Writer writer(&buffer);
    writer.WriteBits(0, 8); // Skip the first byte as it is used by packet

    acServerMessage.Serialize(writer);

    TiltedPhoques::PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());
    Server::Send(aConnectionId, &packet);

    s_allocator.Reset();
}

void GameServer::SendToLoaded(const ServerMessage& acServerMessage) const
{
    auto playerView = m_pWorld->view<const PlayerComponent, const CellIdComponent>();

    for (auto player : playerView)
    {
        const auto& playerComponent = playerView.get<const PlayerComponent>(player);
        Send(playerComponent.ConnectionId, acServerMessage);
    }
}

void GameServer::SendToPlayers(const ServerMessage& acServerMessage) const
{
    auto playerView = m_pWorld->view<const PlayerComponent>();

    for (auto player : playerView)
    {
        const auto& playerComponent = playerView.get<const PlayerComponent>(player);
        Send(playerComponent.ConnectionId, acServerMessage);
    }
}

const String& GameServer::GetName() const noexcept
{
    return m_name;
}

void GameServer::Stop() noexcept
{
    m_requestStop = true;
}

GameServer* GameServer::Get() noexcept
{
	return s_pInstance;
}

void GameServer::HandleAuthenticationRequest(const ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest) noexcept
{
    if(acRequest->Token == m_token)
    {
        auto& scripts = m_pWorld->GetScriptService();

        const auto info = GetConnectionInfo(aConnectionId);

        char remoteAddress[48];

        info.m_addrRemote.ToString(remoteAddress, 48, false);

        // TODO: Abort if a mod didn't accept the player

        auto& registry = *m_pWorld;
        auto& mods = registry.ctx<ModsComponent>();

        const auto cEntity = registry.create();
        registry.emplace<ScriptsComponent>(cEntity);
        auto& playerComponent = registry.emplace<PlayerComponent>(cEntity, aConnectionId);

        playerComponent.Endpoint = remoteAddress;
        playerComponent.DiscordId = acRequest->DiscordId;
        playerComponent.Username = std::move(acRequest->Username);

        AuthenticationResponse serverResponse;

        Mods& serverMods = serverResponse.UserMods;

        // Note: to lower traffic we only send the mod ids the user can fix in order as other ids will lead to a null form id anyway
        std::ostringstream oss;
        oss << "New player {:x} connected with mods\n\t Standard: ";
        for (auto& standardMod : acRequest->UserMods.StandardMods)
        {
            oss << standardMod.Filename << ", ";

            const auto id = mods.AddStandard(standardMod.Filename);

            Mods::Entry entry;
            entry.Filename = standardMod.Filename;
            entry.Id = static_cast<uint16_t>(id);

            playerComponent.Mods.push_back(standardMod.Filename);
            playerComponent.ModIds.push_back(entry.Id);

            serverMods.StandardMods.push_back(entry);
        }

        oss << "\n\t Lite: ";
        for (auto& liteMod : acRequest->UserMods.LiteMods)
        {
            oss << liteMod.Filename << ", ";

            const auto id = mods.AddLite(liteMod.Filename);

            Mods::Entry entry;
            entry.Filename = liteMod.Filename;
            entry.Id = static_cast<uint16_t>(id);

            playerComponent.Mods.push_back(liteMod.Filename);
            playerComponent.ModIds.push_back(entry.Id);


            serverMods.LiteMods.push_back(entry);
        }

        Script::Player player(cEntity, *m_pWorld);
        auto [canceled, reason] = scripts.HandlePlayerJoin(player);

        if (canceled)
        {
            spdlog::info("New player {:x} has a been rejected because \"{}\".", aConnectionId, reason.c_str());

            Kick(aConnectionId);
            m_pWorld->destroy(cEntity);
            return;
        }

        spdlog::info(oss.str(), aConnectionId);

        serverResponse.ServerScripts = std::move(scripts.SerializeScripts());
        serverResponse.ReplicatedObjects = std::move(scripts.GenerateFull());

        Send(aConnectionId, serverResponse);

        m_pWorld->GetDispatcher().trigger(PlayerJoinEvent(cEntity));
    }
    else
    {
        spdlog::info("New player {:x} has a bad token, kicking.", aConnectionId);

        Kick(aConnectionId);
    }
}

void GameServer::SetTitle() const
{
    std::string title(m_name.empty() ? "Private server" : m_name);
    title += " - ";
    title += std::to_string(GetClientCount());
    title += GetClientCount() <= 1 ? " player - " : " players - ";
    title += std::to_string(GetTickRate());
    title += " FPS - " BUILD_BRANCH "@" BUILD_COMMIT;

#if TP_PLATFORM_WINDOWS
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}
