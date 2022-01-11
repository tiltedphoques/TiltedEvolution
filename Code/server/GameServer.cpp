
#include <stdafx.h>
#include <GameServer.h>
#include <Components.h>
#include <Packet.hpp>

#include <Events/AdminPacketEvent.h>
#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Events/PlayerLeaveCellEvent.h>
#include <Events/CharacterRemoveEvent.h>
#include <Events/OwnershipTransferEvent.h>
#include <steam/isteamnetworkingutils.h>

#include <Messages/ClientMessageFactory.h>
#include <Messages/AuthenticationResponse.h>
#include <Scripts/Player.h>

#include <Setting.h>

#if TP_PLATFORM_WINDOWS
#include <windows.h>
#endif

// TODO: this is buggy as fuck with 
static Setting<uint32_t> uServerPort{"general:uPort", "Which port to host the server on", 10578};
static Setting<bool> bPremiumTickrate{"general:bPremium", "Use premium tick rate", true};

GameServer* GameServer::s_pInstance = nullptr;

GameServer::GameServer(String aName, String aToken, String aAdminPassword) noexcept
    : m_lastFrameTime(std::chrono::high_resolution_clock::now()), m_token(std::move(aToken)),
      m_adminPassword(std::move(aAdminPassword)),
      m_requestStop(false)
{
    assert(s_pInstance == nullptr);

    s_pInstance = this;

    const uint16_t userTick = bPremiumTickrate ? 60 : 20;

    auto port = uServerPort.value_as<uint16_t>();
    while (!Host(port, userTick))
    {
        spdlog::warn("Port {} is already in use, trying {}", port, port + 1);
        port++;
    }

    // Fill in Info field.
    m_info.name = std::move(aName);
    m_info.desc = "This is my very own ST server";
    m_info.tick_rate = userTick;
    m_info.token_url = "";

    if (m_info.name.empty())
        m_info.name = "A Skyrim Together Server";

    spdlog::info("Server started on port {}", GetPort());
    UpdateTitle();

    m_pWorld = std::make_unique<World>();

    auto handlerGenerator = [this](auto& x)
    {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_messageHandlers[T::Opcode] = [this](UniquePtr<ClientMessage>& apMessage, ConnectionId_t aConnectionId) {

            auto* pPlayer = m_pWorld->GetPlayerManager().GetByConnectionId(aConnectionId);

            if (!pPlayer)
            {
                spdlog::error("Connection {:x} is not associated with a player.", aConnectionId);
                Kick(aConnectionId);
                return;
            }

            const auto pRealMessage = CastUnique<T>(std::move(apMessage));

            m_pWorld->GetDispatcher().trigger(PacketEvent<T>(pRealMessage.get(), pPlayer));
        };

        return false;
    };

    ClientMessageFactory::Visit(handlerGenerator);

    // Override authentication request
    m_messageHandlers[AuthenticationRequest::Opcode] = [this](UniquePtr<ClientMessage>& apMessage, ConnectionId_t aConnectionId) {
        const auto pRealMessage = CastUnique<AuthenticationRequest>(std::move(apMessage));
        HandleAuthenticationRequest(aConnectionId, pRealMessage);
    };

    auto adminHandlerGenerator = [this](auto& x) {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_adminMessageHandlers[T::Opcode] = [this](UniquePtr<ClientAdminMessage>& apMessage, ConnectionId_t aConnectionId) {
            
            const auto pRealMessage = CastUnique<T>(std::move(apMessage));
            m_pWorld->GetDispatcher().trigger(AdminPacketEvent<T>(pRealMessage.get(), aConnectionId));
        };

        return false;
    };

    //ClientAdminMessageFactory::Visit(adminHandlerGenerator);
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
    ViewBuffer buf((uint8_t*)apData, aSize);
    Buffer::Reader reader(&buf);

    if (m_adminSessions.contains(aConnectionId)) [[unlikely]]
    {
        /* const ClientAdminMessageFactory factory;
        auto pMessage = factory.Extract(reader);
        if (!pMessage)
        {
            spdlog::error("Couldn't parse packet from {:x}", aConnectionId);
            return;
        }*/

        //m_adminMessageHandlers[pMessage->GetOpcode()](pMessage, aConnectionId);
    }
    else
    {
        const ClientMessageFactory factory;
        auto pMessage = factory.Extract(reader);
        if (!pMessage)
        {
            spdlog::error("Couldn't parse packet from {:x}", aConnectionId);
            return;
        }

        m_messageHandlers[pMessage->GetOpcode()](pMessage, aConnectionId);
    }
}

void GameServer::OnConnection(const ConnectionId_t aHandle)
{
    spdlog::info("Connection received {:x}", aHandle);
    UpdateTitle();
}

void GameServer::OnDisconnection(const ConnectionId_t aConnectionId, EDisconnectReason aReason)
{
    spdlog::info("Connection ended {:x}", aConnectionId);

    m_adminSessions.erase(aConnectionId);

    auto* pPlayer = m_pWorld->GetPlayerManager().GetByConnectionId(aConnectionId);

    m_pWorld->GetScriptService().HandlePlayerQuit(aConnectionId, aReason);

    if (pPlayer)
    {
        if (const auto& cell = pPlayer->GetCellComponent())
        {
            const auto oldCell = cell.Cell;
            pPlayer->SetCellComponent(CellIdComponent{{}, {}, {}});
            m_pWorld->GetDispatcher().trigger(PlayerLeaveCellEvent(oldCell));
        }
        m_pWorld->GetDispatcher().trigger(PlayerLeaveEvent(pPlayer));

        entt::entity playerCharacter = pPlayer->GetCharacter().value_or(static_cast<entt::entity>(0));

        // Cleanup all entities that we own
        auto ownerView = m_pWorld->view<OwnerComponent>();
        for (auto entity : ownerView)
        {
            if (entity == playerCharacter)
            {
                auto& characterComponent = m_pWorld->get<CharacterComponent>(entity);
                m_pWorld->GetDispatcher().trigger(CharacterRemoveEvent(World::ToInteger(entity)));
                continue;
            }

            const auto& [ownerComponent] = ownerView.get(entity);
            if (ownerComponent.GetOwner() == pPlayer)
            {
                m_pWorld->GetDispatcher().trigger(OwnershipTransferEvent(entity));
            }
        }
    }

    m_pWorld->GetPlayerManager().Remove(pPlayer);

    UpdateTitle();
}

void GameServer::Send(const ConnectionId_t aConnectionId, const ServerMessage& acServerMessage) const
{
    static thread_local ScratchAllocator s_allocator{ 1 << 18 };

    ScopedAllocator _(s_allocator);

    Buffer buffer(1 << 16);
    Buffer::Writer writer(&buffer);
    writer.WriteBits(0, 8); // Skip the first byte as it is used by packet

    acServerMessage.Serialize(writer);

    PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());
    Server::Send(aConnectionId, &packet);

    s_allocator.Reset();
}

void GameServer::Send(ConnectionId_t aConnectionId, const ServerAdminMessage& acServerMessage) const
{
    static thread_local ScratchAllocator s_allocator{1 << 18};

    ScopedAllocator _(s_allocator);

    Buffer buffer(1 << 16);
    Buffer::Writer writer(&buffer);
    writer.WriteBits(0, 8); // Skip the first byte as it is used by packet

    acServerMessage.Serialize(writer);

    PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());
    Server::Send(aConnectionId, &packet);

    s_allocator.Reset();
}

void GameServer::SendToLoaded(const ServerMessage& acServerMessage) const
{
    for(auto pPlayer : m_pWorld->GetPlayerManager())
    {
        if (pPlayer->GetCellComponent())
            pPlayer->Send(acServerMessage);
    }
}

void GameServer::SendToPlayers(const ServerMessage& acServerMessage) const
{
    for (auto pPlayer : m_pWorld->GetPlayerManager())
    {
        pPlayer->Send(acServerMessage);
    }
}

void GameServer::SendToPlayersInRange(const ServerMessage& acServerMessage, const entt::entity acOrigin) const
{
    const auto* cellIdComp = m_pWorld->try_get<CellIdComponent>(acOrigin);
    const auto* ownerComp = m_pWorld->try_get<OwnerComponent>(acOrigin);

    if (!cellIdComp || !ownerComp)
    {
        spdlog::warn("Components not found for entity {:X}", static_cast<int>(acOrigin));
        return;
    }

    for (auto pPlayer : m_pWorld->GetPlayerManager())
    {
        if (ownerComp->GetOwner() == pPlayer)
            continue;

        if (cellIdComp->IsInRange(pPlayer->GetCellComponent()))
            pPlayer->Send(acServerMessage);
    }
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
    const auto info = GetConnectionInfo(aConnectionId);

    char remoteAddress[48];

    info.m_addrRemote.ToString(remoteAddress, 48, false);

    if(acRequest->Token == m_token)
    {
        auto& scripts = m_pWorld->GetScriptService();

        // TODO: Abort if a mod didn't accept the player

        auto& mods = m_pWorld->ctx<ModsComponent>();

        auto* pPlayer = m_pWorld->GetPlayerManager().Create(aConnectionId);

        pPlayer->SetEndpoint(remoteAddress);
        pPlayer->SetDiscordId(acRequest->DiscordId);
        pPlayer->SetUsername(std::move(acRequest->Username));

        AuthenticationResponse serverResponse;

        Mods& serverMods = serverResponse.UserMods;

        // Note: to lower traffic we only send the mod ids the user can fix in order as other ids will lead to a null form id anyway
        std::ostringstream oss;
        oss << "New player {:x} connected with mods\n\t Standard: ";

        Vector<String> playerMods;
        Vector<uint16_t> playerModsIds;
        for (auto& standardMod : acRequest->UserMods.StandardMods)
        {
            oss << standardMod.Filename << ", ";

            const auto id = mods.AddStandard(standardMod.Filename);

            Mods::Entry entry;
            entry.Filename = standardMod.Filename;
            entry.Id = static_cast<uint16_t>(id);

            playerMods.push_back(standardMod.Filename);
            playerModsIds.push_back(entry.Id);

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

            playerMods.push_back(liteMod.Filename);
            playerModsIds.push_back(entry.Id);

            serverMods.LiteMods.push_back(entry);
        }

        pPlayer->SetMods(playerMods);
        pPlayer->SetModIds(playerModsIds);

        //TODO: Scripting
        /*Script::Player player(cEntity, *m_pWorld);
        auto [canceled, reason] = scripts.HandlePlayerJoin(player);

        if (canceled)
        {
            spdlog::info("New player {:x} has a been rejected because \"{}\".", aConnectionId, reason.c_str());

            Kick(aConnectionId);
            m_pWorld->destroy(cEntity);
            return;
        }*/

        spdlog::info(oss.str(), aConnectionId);

        serverResponse.ServerScripts = std::move(scripts.SerializeScripts());
        serverResponse.ReplicatedObjects = std::move(scripts.GenerateFull());

        Send(aConnectionId, serverResponse);

        m_pWorld->GetDispatcher().trigger(PlayerJoinEvent(pPlayer));
    }
    else if (acRequest->Token == m_adminPassword && !m_adminPassword.empty())
    {
        /* AdminSessionOpen response;
        Send(aConnectionId, response);

        m_adminSessions.insert(aConnectionId);
        spdlog::warn("New admin session for {:x} '{}'", aConnectionId, remoteAddress);*/
    }
    else
    {
        spdlog::info("New player {:x} '{}' has a bad token, kicking.", aConnectionId, remoteAddress);

        Kick(aConnectionId);
    }
}

void GameServer::UpdateTitle() const
{
    const auto name = m_info.name.empty() ? "Private server" : m_info.name;
    const char* playerText = GetClientCount() <= 1 ? " player" : " players";

    constexpr char kFormatText[] = "{} - {} {} - {} Ticks - " BUILD_BRANCH "@" BUILD_COMMIT;
    auto title = fmt::format(kFormatText, name, GetClientCount(), playerText, GetTickRate());

#if TP_PLATFORM_WINDOWS
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}
