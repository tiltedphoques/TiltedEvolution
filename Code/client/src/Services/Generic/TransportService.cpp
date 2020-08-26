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
#include <Services/DiscordService.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <Messages/AuthenticationResponse.h>
#include <Messages/ServerMessageFactory.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/EnterCellRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/NotifyQuestUpdate.h>

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
        ScopedAllocator _{ s_allocator };

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
    TRANSPORT_DISPATCH(ServerTimeSettings);
    TRANSPORT_DISPATCH(CharacterSpawnRequest);
    TRANSPORT_DISPATCH(NotifyInventoryChanges);
    TRANSPORT_DISPATCH(NotifyFactionsChanges);
    TRANSPORT_DISPATCH(NotifyRemoveCharacter);
    TRANSPORT_DISPATCH(NotifyQuestUpdate);

    default:
        spdlog::error("Client message opcode {} from server has no handler", pMessage->GetOpcode());
        break;
    }
}

void TransportService::OnConnected()
{
    AuthenticationRequest request;

    // null if discord is not active
    // TODO: think about user opt out
    request.DiscordId = m_world.ctx<DiscordService>().GetUser().id;

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
        EnterCellRequest message;
        message.CellId = GameId(modId, baseId);

        Send(message);
    }
}

void TransportService::OnDraw() noexcept
{
    if (m_connected)
    {
        ImGui::Begin("Network");
        auto status = GetConnectionStatus();
        status.m_flOutBytesPerSec /= 1024.f;
        status.m_flInBytesPerSec /= 1024.f;

        ImGui::InputFloat("Net Out kBps", (float*)&status.m_flOutBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Net In kBps", (float*)&status.m_flInBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        auto stats = GetStatistics();
        float protocolSent = float(stats.SentBytes) / 1024.f;
        float protocolReceived = float(stats.RecvBytes) / 1024.f;
        float uncompressedSent = float(stats.UncompressedSentBytes) / 1024.f;
        float uncompressedReceived = float(stats.UncompressedRecvBytes) / 1024.f;

        ImGui::InputFloat("Protocol Out kBps", (float*)&protocolSent, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Protocol In kBps", (float*)&protocolReceived, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        ImGui::InputFloat("User Out kBps", (float*)&uncompressedSent, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("User In kBps", (float*)&uncompressedReceived, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::End();
    }

    // online indicator
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(23.f, 23.f), 
        ImVec2(50.f, 50.f), m_connected ? ImColor(0, 230, 64) : ImColor(240, 52, 52));
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
