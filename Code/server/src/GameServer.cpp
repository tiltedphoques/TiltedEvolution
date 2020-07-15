#include <GameServer.h>
#include <Components.h>
#include <Packet.hpp>

#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>
#include <Messages/ClientMessageFactory.h>
#include <Messages/AuthenticationRequest.h>
#include <Messages/AuthenticationResponse.h>

#include <Scripts/Player.h>

#define SERVER_HANDLE(packetName, functionName) case TiltedMessages::ClientMessage::k##packetName: Handle##packetName(aConnectionId, message.functionName()); break;
#define SERVER_DISPATCH(packetName, functionName) case TiltedMessages::ClientMessage::k##packetName: dispatcher.trigger(PacketEvent<TiltedMessages::packetName>(message.mutable_##functionName(), aConnectionId)); break;

GameServer* GameServer::s_pInstance = nullptr;

GameServer::GameServer(uint16_t aPort, bool aPremium, String aName, String aToken) noexcept
    : m_lastFrameTime(std::chrono::high_resolution_clock::now())
    , m_name(std::move(aName))
    , m_token(std::move(aToken))
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    assert(s_pInstance == nullptr);

    s_pInstance = this;

    while (!Host(aPort, aPremium ? 60 : 20))
    {
        spdlog::warn("Port {} is already in use, trying {}", aPort, aPort + 1);
        aPort++;
    }

    spdlog::info("Server started on port {}", GetPort());

    SetTitle();
}

GameServer::~GameServer()
{
    s_pInstance = nullptr;
}

void GameServer::OnUpdate()
{
    const auto cNow = std::chrono::high_resolution_clock::now();
    const auto cDelta = cNow - m_lastFrameTime;
    m_lastFrameTime = cNow;

    const auto cDeltaSeconds = std::chrono::duration_cast<std::chrono::duration<float>>(cDelta).count();

    auto& dispatcher = m_world.GetDispatcher();

    dispatcher.trigger(UpdateEvent{cDeltaSeconds});
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

    auto& dispatcher = m_world.GetDispatcher();

    switch(pMessage->GetOpcode())
    {
    case kAuthenticationRequest:
    {
        const auto pRealMessage = CastUnique<AuthenticationRequest>(std::move(pMessage));
        HandleAuthenticationRequest(aConnectionId, pRealMessage);
    }
        break;
    default:
        spdlog::error("Client message opcode {} from {:x} has no handler", pMessage->GetOpcode(), aConnectionId);
        break;
    }
}

void GameServer::OnConnection(const ConnectionId_t aHandle)
{
    spdlog::info("Connection received {:x}", aHandle);

    SetTitle();
}

void GameServer::OnDisconnection(const ConnectionId_t aConnectionId)
{
    StackAllocator<1 << 14> allocator;
    ScopedAllocator _{ allocator };

    spdlog::info("Connection ended {:x}", aConnectionId);

    m_world.GetScriptService().HandlePlayerQuit(aConnectionId);

    auto& registry = m_world;

    Vector<entt::entity> entitiesToDestroy;
    entitiesToDestroy.reserve(500);

    // Find if a player is associated with this connection and delete it
    auto playerView = registry.view<PlayerComponent>();
    for (auto entity : playerView)
    {
        const auto& playerComponent = playerView.get(entity);
        if (playerComponent.ConnectionId == aConnectionId)
        {
            entitiesToDestroy.push_back(entity);
            break;
        }
    }

    // Cleanup all entities that we own
    auto ownerView = registry.view<OwnerComponent>();
    for (auto entity : ownerView)
    {
        const auto& ownerComponent = ownerView.get(entity);
        if (ownerComponent.ConnectionId == aConnectionId)
        {
            entitiesToDestroy.push_back(entity);
        }
    }

    for(auto entity : entitiesToDestroy)
    {
        registry.remove_if_exists<ScriptsComponent>(entity);
        registry.destroy(entity);
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

    TiltedPhoques::PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.GetBytePosition());
    Server::Send(aConnectionId, &packet);

    s_allocator.Reset();
}

void GameServer::SendToLoaded(const ServerMessage& acServerMessage) const
{
    auto playerView = m_world.view<const PlayerComponent, const CellIdComponent>();

    for (auto player : playerView)
    {
        const auto& playerComponent = playerView.get<const PlayerComponent>(player);
        Send(playerComponent.ConnectionId, acServerMessage);
    }
}

GameServer* GameServer::Get() noexcept
{
	return s_pInstance;
}

void GameServer::HandleAuthenticationRequest(const ConnectionId_t aConnectionId, const UniquePtr<AuthenticationRequest>& acRequest) noexcept
{
    if(acRequest->Token == m_token)
    {
        auto& scripts = m_world.GetScriptService();

        const auto info = GetConnectionInfo(aConnectionId);

        char remoteAddress[48];

        info.m_addrRemote.ToString(remoteAddress, 48, false);

        // TODO: Abort if a mod didn't accept the player

        auto& registry = m_world;
        auto& mods = registry.ctx<ModsComponent>();

        const auto cEntity = registry.create();
        registry.emplace<ScriptsComponent>(cEntity);
        auto& playerComponent = registry.emplace<PlayerComponent>(cEntity, aConnectionId);

        playerComponent.Endpoint = remoteAddress;

        AuthenticationResponse serverResponse;

        Mods& serverMods = serverResponse.Mods;

        // Note: to lower traffic we only send the mod ids the user can fix in order as other ids will lead to a null form id anyway
        std::ostringstream oss;
        oss << "New player {:x} connected with mods\n\t Standard: ";
        for (auto& standardMod : acRequest->Mods.StandardMods)
        {
            oss << standardMod.Filename << ", ";

            playerComponent.Mods.push_back(standardMod.Filename);
            const auto id = mods.AddStandard(standardMod.Filename);

            Mods::Entry entry;
            entry.Filename = standardMod.Filename;
            entry.Id = id;

            serverMods.StandardMods.push_back(entry);
        }

        oss << "\n\t Lite: ";
        for (auto& liteMod : acRequest->Mods.LiteMods)
        {
            oss << liteMod.Filename << ", ";

            playerComponent.Mods.push_back(liteMod.Filename);
            const auto id = mods.AddLite(liteMod.Filename);

            Mods::Entry entry;
            entry.Filename = liteMod.Filename;
            entry.Id = id;

            serverMods.LiteMods.push_back(entry);
        }

        Script::Player player(cEntity, m_world);
        auto [canceled, reason] = scripts.HandlePlayerConnect(player);

        if (canceled)
        {
            spdlog::info("New player {:x} has a been rejected because \"{}\".", aConnectionId, reason.c_str());

            Kick(aConnectionId);

            m_world.destroy(cEntity);

            return;
        }

        spdlog::info(oss.str(), aConnectionId);

        serverResponse.Scripts.Data = scripts.SerializeScripts();
        serverResponse.ReplicatedObjects.Data = scripts.GenerateFull();

        Send(aConnectionId, serverResponse);
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
    title += " FPS";

#if TP_PLATFORM_WINDOWS
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}
