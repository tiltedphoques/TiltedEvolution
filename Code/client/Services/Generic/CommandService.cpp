#include <Services/CommandService.h>
#include <Services/TransportService.h>

#include <Systems/ModSystem.h>
#include <World.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <PlayerCharacter.h>

#include <Events/SetTimeCommandEvent.h>
#include <Events/ConnectionErrorEvent.h>

#include <Messages/SetTimeCommandRequest.h>
#include <Messages/NotifySetTimeResult.h>
#include <Messages/TeleportCommandResponse.h>

#include <Structs/GridCellCoords.h>

CommandService::CommandService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher) 
    : m_world(aWorld), 
      m_transport(aTransport),
      m_dispatcher(aDispatcher)
{
    m_setTimeConnection = aDispatcher.sink<SetTimeCommandEvent>().connect<&CommandService::OnSetTimeCommand>(this);
    m_notifySetTimeConnection = aDispatcher.sink<NotifySetTimeResult>().connect<&CommandService::OnNotifySetTimeResult>(this);
    m_teleportConnection = aDispatcher.sink<TeleportCommandResponse>().connect<&CommandService::OnTeleportCommandResponse>(this);
}

void CommandService::OnSetTimeCommand(const SetTimeCommandEvent& acEvent) const noexcept
{
    SetTimeCommandRequest request{};
    request.Hours = acEvent.Hours;
    request.Minutes = acEvent.Minutes;
    m_transport.Send(request);
}

void CommandService::OnNotifySetTimeResult(const NotifySetTimeResult& acMessage) const noexcept
{
    ConnectionErrorEvent errorEvent{};

    switch (acMessage.Result)
    {
    case NotifySetTimeResult::SetTimeResult::kSuccess:
        m_world.GetOverlayService().SendSystemMessage("SetTime command succeeded.");
        break;
    case NotifySetTimeResult::SetTimeResult::kPublicServer:
        errorEvent.ErrorDetail = "{\"error\": \"set_time_public_server\"}";
        break;
    default:
        break;
    }

    if (!errorEvent.ErrorDetail.empty())
    {
        m_dispatcher.trigger(errorEvent);
        spdlog::error(errorEvent.ErrorDetail.c_str());
    }
}

void CommandService::OnTeleportCommandResponse(const TeleportCommandResponse& acMessage) noexcept
{
    spdlog::info("TeleportCommandResponse: {:X}:{:X} at position {}, {}, {}", acMessage.CellId.ModId, acMessage.CellId.BaseId, acMessage.Position.x, acMessage.Position.y, acMessage.Position.z);
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
