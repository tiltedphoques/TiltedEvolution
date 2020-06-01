#include <GameServer.h>
#include <Components.h>
#include <Packet.hpp>

#include <Events/PacketEvent.h>
#include <Events/UpdateEvent.h>

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
    TiltedMessages::ClientMessage message;
    if(!message.ParseFromArray(apData, aSize))
    {
        spdlog::error("Couldn't parse packet from {:x}", aConnectionId);
        return;
    }

    auto& dispatcher = m_world.GetDispatcher();

    switch(message.Content_case())
    {
        SERVER_HANDLE(AuthenticationRequest, authentication_request);
        SERVER_DISPATCH(CharacterAssignRequest, character_assign_request);
        SERVER_DISPATCH(CancelCharacterAssignRequest, cancel_character_assign_request);
        SERVER_DISPATCH(CellEnterRequest, cell_enter_request);
        SERVER_DISPATCH(ReferenceMovementSnapshot, reference_movement_snapshot);
        SERVER_DISPATCH(RpcCallsRequest, rpc_calls_request);
    case TiltedMessages::ClientMessage::CONTENT_NOT_SET:
        spdlog::error("Client message from {:x} was empty", aConnectionId);
        break;
    default:
        spdlog::error("Client message opcode {} from {:x} has no handler", message.Content_case(), aConnectionId);
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

void GameServer::Send(const ConnectionId_t aConnectionId, const TiltedMessages::ServerMessage& acServerMessage) const
{
    static thread_local TiltedPhoques::ScratchAllocator s_allocator{ 1 << 18 };

    TiltedPhoques::ScopedAllocator _(s_allocator);

    TiltedPhoques::Packet packet(acServerMessage.ByteSize());
    if (acServerMessage.SerializeToArray(packet.GetData(), static_cast<int>(packet.GetSize() & 0x7FFFFFFF)))
        Server::Send(aConnectionId, &packet);
    else
        spdlog::error("Unable to serialize message {}", acServerMessage.Content_case());

    s_allocator.Reset();
}

void GameServer::SendToLoaded(const TiltedMessages::ServerMessage& acServerMessage) const
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

void GameServer::HandleAuthenticationRequest(const ConnectionId_t aConnectionId, const TiltedMessages::AuthenticationRequest& acRequest) noexcept
{
    if(strcmp(acRequest.token().c_str(), m_token.c_str()) == 0)
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

        TiltedMessages::ServerMessage response;
        const auto pAuthenticationResponse = response.mutable_authentication_response();
        const auto pMods = pAuthenticationResponse->mutable_mods();
        const auto pScripts = pAuthenticationResponse->mutable_scripts();
        const auto pReplicatedObjects = pAuthenticationResponse->mutable_objects();

        // Note: to lower traffic we only send the mod ids the user can fix in order as other ids will lead to a null form id anyway
        std::ostringstream oss;
        oss << "New player {:x} connected with mods\n\t Standard: ";
        for (auto& standardMod : acRequest.mods().standard_mods())
        {
            oss << standardMod.filename() << ", ";

            playerComponent.Mods.push_back(standardMod.filename().c_str());
            const auto id = mods.AddStandard(standardMod.filename().c_str());

            auto pMod = pMods->add_standard_mods();
            pMod->set_filename(standardMod.filename().c_str());
            pMod->set_id(id);
        }

        oss << "\n\t Lite: ";
        for (auto& liteMod : acRequest.mods().lite_mods())
        {
            oss << liteMod.filename() << ", ";

            playerComponent.Mods.push_back(liteMod.filename().c_str());
            const auto id = mods.AddLite(liteMod.filename().c_str());

            auto pMod = pMods->add_lite_mods();
            pMod->set_filename(liteMod.filename().c_str());
            pMod->set_id(id);
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

        scripts.Serialize(pScripts);
        scripts.Serialize(pReplicatedObjects);

        Send(aConnectionId, response);
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
