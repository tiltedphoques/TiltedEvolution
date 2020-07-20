#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/CellChangeEvent.h>

#include <Games/TES.h>

#include <World.h>

#include <Packet.hpp>
#include <Messages/AuthenticationRequest.h>
#include <ScratchAllocator.hpp>
#include <Services/ImguiService.h>
#include <imgui.h>

#include <Messages/AuthenticationResponse.h>
#include <Messages/ServerMessageFactory.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>

#define TRANSPORT_DISPATCH(packetName) \
case k##packetName: \
    { \
    const auto pRealMessage = TiltedPhoques::CastUnique<packetName>(std::move(pMessage)); \
    m_dispatcher.trigger(*pRealMessage); \
    } \
    break; 

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

bool TransportService::Send(const ClientMessage& acMessage) const noexcept
{
    static thread_local ScratchAllocator s_allocator(1 << 18);

    struct ScopedReset
    {
        ~ScopedReset() { s_allocator.Reset(); }
    } allocatorGuard;

    if (IsConnected())
    {
        Buffer buffer(1 << 16);
        Buffer::Writer writer(&buffer);
        writer.WriteBits(0, 8); // Write first byte as packet needs it

        acMessage.Serialize(writer);
        TiltedPhoques::PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());

        Client::Send(&packet);

        return true;
    }

    return false;
}

void TransportService::OnConsume(const void* apData, uint32_t aSize)
{
    ServerMessageFactory factory;
    TiltedPhoques::ViewBuffer buf((uint8_t*)apData, aSize);
    Buffer::Reader reader(&buf);

    auto pMessage = factory.Extract(reader);
    if (!pMessage)
    {
        spdlog::error("Couldn't parse packet from server");
        return;
    }

    switch (pMessage->GetOpcode())
    {
    case kAuthenticationResponse:
    {
        const auto pRealMessage = TiltedPhoques::CastUnique<AuthenticationResponse>(std::move(pMessage));
        HandleAuthenticationResponse(*pRealMessage);
    }
    break;

    TRANSPORT_DISPATCH(AssignCharacterResponse);
    TRANSPORT_DISPATCH(ServerReferencesMoveRequest);

    default:
        spdlog::error("Client message opcode {} from server has no handler", pMessage->GetOpcode());
        break;
    }
}

void TransportService::OnConnected()
{
    AuthenticationRequest request;

    const auto cpModManager = ModManager::Get();

    for (auto pMod : cpModManager->mods)
    {
        if (!pMod->IsLoaded())
            continue;

        auto& entry = pMod->IsLite() ? request.Mods.LiteMods.emplace_back() : request.Mods.StandardMods.emplace_back();

        entry.Id = pMod->GetId();
        entry.Filename = pMod->filename;
    }

    Send(request);
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

        //Send(message);
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

void TransportService::HandleAuthenticationResponse(const AuthenticationResponse& acMessage) noexcept
{
    m_connected = true;

    // Dispatch the mods to anyone who needs it
    m_dispatcher.trigger(acMessage.Mods);

    // Dispatch the scripts to anyone who needs it
    m_dispatcher.trigger(acMessage.Scripts);

    // Dispatch the replicated objects
    m_dispatcher.trigger(acMessage.ReplicatedObjects);

    // Notify we are ready for action
    m_dispatcher.trigger(ConnectedEvent());
}
