
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/GridCellChangeEvent.h>
#include <Events/CellChangeEvent.h>

#include <Games/TES.h>
#include <Games/References.h>

#include <Forms/TESNPC.h>
#include <TiltedOnlinePCH.h>
#include <World.h>

#include <Packet.hpp>
#include <Messages/AuthenticationRequest.h>
#include <Messages/ServerMessageFactory.h>
#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>

#include <Services/ImguiService.h>
#include <Services/DiscordService.h>

#include <imgui.h>
//#include <imgui_internal.h>

using TiltedPhoques::Packet;

TransportService::TransportService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TransportService::HandleUpdate>(this);
    m_gridCellChangeConnection = m_dispatcher.sink<GridCellChangeEvent>().connect<&TransportService::OnGridCellChangeEvent>(this);
    m_cellChangeConnection = m_dispatcher.sink<CellChangeEvent>().connect<&TransportService::OnCellChangeEvent>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TransportService::OnDraw>(this);

    m_connected = false;

    auto handlerGenerator = [this](auto& x) {
        using T = typename std::remove_reference_t<decltype(x)>::Type;

        m_messageHandlers[T::Opcode] = [this](UniquePtr<ServerMessage>& apMessage) {
            const auto pRealMessage = TiltedPhoques::CastUnique<T>(std::move(apMessage));
            m_dispatcher.trigger(*pRealMessage);
        };

        return false;
    };

    ServerMessageFactory::Visit(handlerGenerator);

    // Override authentication response
    m_messageHandlers[AuthenticationResponse::Opcode] = [this](UniquePtr<ServerMessage>& apMessage) {
        const auto pRealMessage = TiltedPhoques::CastUnique<AuthenticationResponse>(std::move(apMessage));
        HandleAuthenticationResponse(*pRealMessage);
    };
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

    m_messageHandlers[pMessage->GetOpcode()](pMessage);
}

void TransportService::OnConnected()
{
    AuthenticationRequest request;

    // null if discord is not active
    // TODO: think about user opt out
    request.DiscordId = m_world.ctx<DiscordService>().GetUser().id;
    auto* pNpc = RTTI_CAST(PlayerCharacter::Get()->baseForm, TESForm, TESNPC);
    if (pNpc)
    {
        request.Username = pNpc->fullName.value.AsAscii();
    }
    else
    {
        request.Username = "Some dragon boi";
    }

    const auto cpModManager = ModManager::Get();

    for (auto pMod : cpModManager->mods)
    {
        if (!pMod->IsLoaded())
            continue;

        auto& entry =
            pMod->IsLite() ? request.UserMods.LiteMods.emplace_back() : request.UserMods.StandardMods.emplace_back();

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

void TransportService::OnGridCellChangeEvent(const GridCellChangeEvent& acEvent) const noexcept
{
    uint32_t baseId = 0;
    uint32_t modId = 0;

    if (m_world.GetModSystem().GetServerModId(acEvent.WorldSpaceId, modId, baseId))
    {
        ShiftGridCellRequest request;
        request.WorldSpaceId = GameId(modId, baseId);
        request.PlayerCell = acEvent.PlayerCell;
        request.CenterCoords = acEvent.CenterCoords;
        request.PlayerCoords = acEvent.PlayerCoords;
        request.Cells = acEvent.Cells;

        Send(request);
    }
}

void TransportService::OnCellChangeEvent(const CellChangeEvent& acEvent) const noexcept
{
    if (acEvent.WorldSpaceId != GameId{})
    {
        EnterExteriorCellRequest message;
        message.CellId = acEvent.CellId;
        message.WorldSpaceId = acEvent.WorldSpaceId;
        message.CurrentCoords = acEvent.CurrentCoords;

        Send(message);
    }
    else
    {
        EnterInteriorCellRequest message;
        message.CellId = acEvent.CellId;

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
    m_dispatcher.trigger(acMessage.UserMods);

    // Dispatch the scripts to anyone who needs it
    m_dispatcher.trigger(acMessage.ServerScripts);

    // Dispatch the replicated objects
    m_dispatcher.trigger(acMessage.ReplicatedObjects);

    // Notify we are ready for action
    m_dispatcher.trigger(ConnectedEvent());
}
