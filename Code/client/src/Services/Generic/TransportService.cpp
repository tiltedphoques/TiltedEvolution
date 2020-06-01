#include <Services/TransportService.h>

#include <client_server.pb.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/CellChangeEvent.h>

#include <Games/TES.h>

#include <World.h>

#include <Packet.hpp>
#include <ScratchAllocator.hpp>
#include <Services/ImguiService.h>
#include <imgui.h>

#define TRANSPORT_HANDLE(packetName, functionName) case TiltedMessages::ServerMessage::k##packetName: Handle##packetName(message.functionName()); break;
#define TRANSPORT_DISPATCH(packetName, functionName) case TiltedMessages::ServerMessage::k##packetName: dispatcher.trigger(message.functionName()); break;

using TiltedPhoques::Packet;

TransportService::TransportService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TransportService::HandleUpdate>(this);
    m_cellChangeConnection = m_dispatcher.sink<CellChangeEvent>().connect<&TransportService::OnCellChangeEvent>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TransportService::OnDraw>(this);

    m_connected = false;
}

bool TransportService::Send(const TiltedMessages::ClientMessage& acMessage) const noexcept
{
    static thread_local ScratchAllocator s_allocator(1 << 18);

    struct ScopedReset
    {
        ~ScopedReset() { s_allocator.Reset(); }
    } allocatorGuard;

    if (IsConnected())
    {
        ScopedAllocator _{s_allocator};

        // We first try to allocate using the scratch pool
        auto pPacket = TiltedPhoques::MakeUnique<Packet>(acMessage.ByteSize());

        // If it failed to allocate the packet object or the underlying buffer, fallback to the default allocator (usually malloc/free)
        if(!pPacket || !pPacket->IsValid())
            pPacket.reset(Allocator::GetDefault()->New<Packet>(acMessage.ByteSize()));

        if (pPacket && pPacket->IsValid() && acMessage.SerializeToArray(pPacket->GetData(), static_cast<int>(pPacket->GetSize())))
        {
            Client::Send(pPacket.get());

            return true;
        }
        else
        {
            // TODO: Understand wtf is going on here
        }
    }

    return false;
}

void TransportService::OnConsume(const void* apData, uint32_t aSize)
{
    TiltedMessages::ServerMessage message;
    if (!message.ParseFromArray(apData, aSize))
    {
        return;
    }

    auto& dispatcher = m_dispatcher;

    switch (message.Content_case())
    {
        TRANSPORT_HANDLE(AuthenticationResponse, authentication_response);
        TRANSPORT_DISPATCH(StringCacheContent, string_cache_content);
        TRANSPORT_DISPATCH(CharacterAssignResponse, character_assign_response);
        TRANSPORT_DISPATCH(CharacterSpawnRequest, character_spawn_request);
        TRANSPORT_DISPATCH(ReferenceMovementSnapshot, reference_movement_snapshot);
        TRANSPORT_DISPATCH(ReplicatedNetObjects, replicated_net_objects);
    default:
        assert(false);
        break;
    }
}

void TransportService::OnConnected()
{
    TiltedMessages::ClientMessage message;
    const auto cpAuthenticationRequest = message.mutable_authentication_request();
    cpAuthenticationRequest->set_token("");
    const auto cpMods = cpAuthenticationRequest->mutable_mods();

    const auto cpModManager = ModManager::Get();

    for (auto pMod : cpModManager->mods)
    {
        if (!pMod->IsLoaded())
            continue;

        const auto cpEntry = pMod->IsLite() ? cpMods->add_lite_mods() : cpMods->add_standard_mods();

        cpEntry->set_filename(pMod->filename);
        cpEntry->set_id(pMod->GetId());
    }

    Send(message);
}

void TransportService::OnDisconnected(EDisconnectReason aReason)
{
    m_connected = false;

    spdlog::warn("Disconnected from server {}", aReason);

    m_dispatcher.trigger(DisconnectedEvent());
}

void TransportService::OnUpdate()
{
}

void TransportService::HandleUpdate(const UpdateEvent& acEvent) noexcept
{
    Update();
}

void TransportService::OnCellChangeEvent(const CellChangeEvent& acEvent) const noexcept
{
    uint32_t baseId = 0;
    uint32_t modId = 0;

    if(m_world.GetModSystem().GetServerModId(acEvent.CellId, modId, baseId))
    {
        TiltedMessages::ClientMessage message;
        const auto pCellEnterRequest = message.mutable_cell_enter_request();
        auto pCellId = pCellEnterRequest->mutable_cell_id();

        pCellId->set_mod(modId);
        pCellId->set_base(baseId);

        Send(message);
    }
}

void TransportService::OnDraw() noexcept
{
    if(IsOnline())
    {
        ImGui::Begin("Network");

        auto status = GetConnectionStatus();
        status.m_flOutBytesPerSec /= 1024;
        status.m_flInBytesPerSec /= 1024;

        ImGui::InputFloat("Out kBps", (float*)&status.m_flOutBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("In kBps", (float*)&status.m_flInBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        ImGui::End();
    }
}

void TransportService::HandleAuthenticationResponse(const TiltedMessages::AuthenticationResponse& acMessage) noexcept
{
    m_connected = true;

    // Dispatch the mods to anyone who needs it
    m_dispatcher.trigger(acMessage.mods());

    // Dispatch the scripts to anyone who needs it
    m_dispatcher.trigger(acMessage.scripts());

    // Dispatch the replicated objects
    m_dispatcher.trigger(acMessage.objects());

    // Notify we are ready for action
    m_dispatcher.trigger(ConnectedEvent());
}
