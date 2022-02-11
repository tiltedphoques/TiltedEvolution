
#include <Components.h>
#include <GameServer.h>
#include <Packet.hpp>
#include <stdafx.h>

#include <Events/AdminPacketEvent.h>
#include <Events/CharacterRemoveEvent.h>
#include <Events/OwnershipTransferEvent.h>
#include <Events/PacketEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveCellEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Events/UpdateEvent.h>
#include <steam/isteamnetworkingutils.h>

#include <AdminMessages/ClientAdminMessageFactory.h>
#include <Messages/AuthenticationResponse.h>
#include <Messages/ClientMessageFactory.h>
#include <console/ConsoleRegistry.h>

#if TP_PLATFORM_WINDOWS
#include <windows.h>
#endif

// >> Game server cvars <<
static Console::Setting uServerPort{"GameServer:uPort", "Which port to host the server on", 10578u};
static Console::Setting bPremiumTickrate{"GameServer:bPremiumMode", "Use premium tick rate", true};
static Console::StringSetting sServerName{"GameServer:sServerName", "Name that shows up in the server list",
                                          "Dedicated Together Server"};
static Console::StringSetting sServerDesc{"GameServer:sServerDesc", "Description that shows up in the server list",
                                          "Hello there!"};
static Console::StringSetting sServerIconURL{"GameServer:sIconUrl", "URL to the image that shows up in the server list",
                                             ""};
static Console::StringSetting sTagList{"GameServer:sTagList", "List of tags, separated by a comma (,)", ""};
static Console::StringSetting sAdminPassword{"GameServer:sAdminPassword", "Admin authentication password", ""};
static Console::StringSetting sToken{"GameServer:sToken", "Admin token", ""};

// >> Constants <<
static constexpr size_t kTagListCap = 512;

static Console::Command<bool> TogglePremium("TogglePremium", "Toggle the premium mode",
                                            [](Console::ArgStack& aStack) { bPremiumTickrate = aStack.Pop<bool>(); });

static Console::Command<> ShowVersion("version", "Show the version the server was compiled with",
                                      [](Console::ArgStack&) { spdlog::get("ConOut")->info("Server " BUILD_COMMIT); });

static uint16_t GetUserTickRate()
{
    return bPremiumTickrate ? 60 : 30;
}

GameServer* GameServer::s_pInstance = nullptr;

GameServer::GameServer(Console::ConsoleRegistry* apConsole) noexcept
    : m_lastFrameTime(std::chrono::high_resolution_clock::now()), m_pCommands(apConsole), m_requestStop(false)
{
    BASE_ASSERT(s_pInstance == nullptr, "Server instance already exists?");
    s_pInstance = this;

    auto port = uServerPort.value_as<uint16_t>();
    while (!Host(port, GetUserTickRate()))
    {
        spdlog::warn("Port {} is already in use, trying {}", port, port + 1);
        port++;
    }

    UpdateInfo();
    spdlog::info("Server started on port {}", GetPort());
    UpdateTitle();

    m_pWorld = std::make_unique<World>();

    BindMessageHandlers();
}

GameServer::~GameServer()
{
    s_pInstance = nullptr;
}

GameServer* GameServer::Get() noexcept
{
    return s_pInstance;
}

void GameServer::Initialize()
{
    BindServerCommands();
    m_pWorld->GetScriptService().Initialize();
}

void GameServer::Kill()
{
    spdlog::info("Server shutdown requested");
    m_requestStop = true;
}

void GameServer::BindMessageHandlers()
{
    auto handlerGenerator = [this](auto& x) {
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
    m_messageHandlers[AuthenticationRequest::Opcode] = [this](UniquePtr<ClientMessage>& apMessage,
                                                              ConnectionId_t aConnectionId) {
        const auto pRealMessage = CastUnique<AuthenticationRequest>(std::move(apMessage));
        HandleAuthenticationRequest(aConnectionId, pRealMessage);
    };

    auto adminHandlerGenerator = [this](auto& x) {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_adminMessageHandlers[T::Opcode] = [this](UniquePtr<ClientAdminMessage>& apMessage,
                                                   ConnectionId_t aConnectionId) {
            const auto pRealMessage = CastUnique<T>(std::move(apMessage));
            m_pWorld->GetDispatcher().trigger(AdminPacketEvent<T>(pRealMessage.get(), aConnectionId));
        };

        return false;
    };

    ClientAdminMessageFactory::Visit(adminHandlerGenerator);
}

void GameServer::BindServerCommands()
{
    BASE_ASSERT(m_pCommands, "Command logic error");

    m_pCommands->RegisterCommand<>("players", "List all players on this server", [&](Console::ArgStack&) {
        auto out = spdlog::get("ConOut");
        uint32_t count = m_pWorld->GetPlayerManager().Count();
        if (count == 0)
        {
            out->warn("No players on here. Invite some friends!");
            return;
        }

        out->info("<------Players-({})--->", count);
        for (Player* pPlayer : m_pWorld->GetPlayerManager())
        {
            out->info("{}: {}", pPlayer->GetId(), pPlayer->GetUsername().c_str());
        }
    });

    m_pCommands->RegisterCommand<>("quit", "Stop the server", [&](Console::ArgStack&) { Kill(); });
}

void GameServer::UpdateInfo()
{
    // Update Info fields from user facing CVARS.
    m_info.name = sServerName.c_str();
    m_info.desc = sServerDesc.c_str();
    m_info.icon_url = sServerIconURL.c_str();
    m_info.tick_rate = GetUserTickRate();

    if (std::strlen(sTagList.c_str()) > 512)
    {
        m_info.tagList = "";
        spdlog::warn("TagList limit exceeded (512 characters). Ignoring tags.");
    }
    else
        m_info.tagList = sTagList.c_str();
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
        const ClientAdminMessageFactory factory;
        auto pMessage = factory.Extract(reader);
        if (!pMessage)
        {
            spdlog::error("Couldn't parse packet from {:x}", aConnectionId);
            return;
        }

        m_adminMessageHandlers[pMessage->GetOpcode()](pMessage, aConnectionId);
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
    for (auto pPlayer : m_pWorld->GetPlayerManager())
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

void GameServer::HandleAuthenticationRequest(const ConnectionId_t aConnectionId,
                                             const UniquePtr<AuthenticationRequest>& acRequest)
{
    const auto info = GetConnectionInfo(aConnectionId);

    char remoteAddress[48];

    info.m_addrRemote.ToString(remoteAddress, 48, false);

    if (acRequest->Token == sToken.value())
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

        // Note: to lower traffic we only send the mod ids the user can fix in order as other ids will lead to a null
        // form id anyway
        std::ostringstream oss;
        oss << fmt::format("New player {:x} connected with mods\n\t Standard: ", aConnectionId);

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

        // TODO: Scripting
        /*Script::Player player(cEntity, *m_pWorld);
        auto [canceled, reason] = scripts.HandlePlayerJoin(player);

        if (canceled)
        {
            spdlog::info("New player {:x} has a been rejected because \"{}\".", aConnectionId, reason.c_str());

            Kick(aConnectionId);
            m_pWorld->destroy(cEntity);
            return;
        }*/

        spdlog::info("{}", oss.str().c_str());

        serverResponse.ServerScripts = std::move(scripts.SerializeScripts());
        serverResponse.ReplicatedObjects = std::move(scripts.GenerateFull());

        Send(aConnectionId, serverResponse);

        m_pWorld->GetDispatcher().trigger(PlayerJoinEvent(pPlayer));
    }
    else if (acRequest->Token == sAdminPassword.value() && !sAdminPassword.empty())
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

    const auto title = fmt::format("{} - {} {} - {} Ticks - " BUILD_BRANCH "@" BUILD_COMMIT, name.c_str(),
                                   GetClientCount(), playerText, GetTickRate());

#if TP_PLATFORM_WINDOWS
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}
