#include <Services/CommandService.h>
#include <Services/TransportService.h>

#include <Systems/ModSystem.h>
#include <World.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <PlayerCharacter.h>

#include <Events/CommandEvent.h>

#include <Messages/TeleportCommandRequest.h>
#include <Messages/TeleportCommandResponse.h>

#include <Structs/GridCellCoords.h>

CommandService::CommandService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher) 
    : m_world(aWorld), 
      m_transport(aTransport)
{
    m_commandEventConnection = aDispatcher.sink<CommandEvent>().connect<&CommandService::OnCommandEvent>(this);
    m_teleportConnection = aDispatcher.sink<TeleportCommandResponse>().connect<&CommandService::OnTeleportCommandResponse>(this);
}

void CommandService::OnCommandEvent(const CommandEvent& acEvent) noexcept
{
    const size_t commandWordPos = acEvent.Command.find(' ');
    String commandWord = acEvent.Command.substr(1, commandWordPos - 1);

    if (commandWord == "tp")
    {
        TeleportCommandRequest request{};
        request.TargetPlayer = acEvent.Command.substr(commandWordPos + 1, acEvent.Command.size());

        m_transport.Send(request);

        spdlog::info("Target player: '{}'", request.TargetPlayer);
    }
    else
    {
        spdlog::warn("Command word does not exist: {}", commandWord);
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
