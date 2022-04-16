#include <Services/CommandService.h>
#include <Services/TransportService.h>

#include <Systems/ModSystem.h>
#include <World.h>

#include <Forms/TESObjectCELL.h>
#include <PlayerCharacter.h>

#include <Events/CommandEvent.h>

#include <Messages/TeleportCommandRequest.h>
#include <Messages/TeleportCommandResponse.h>

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

        spdlog::info("Target player: '{}'", request.TargetPlayer);

        m_transport.Send(request);
    }
    else
    {
        spdlog::warn("Command word does not exist: {}", commandWord);
    }
}

void CommandService::OnTeleportCommandResponse(const TeleportCommandResponse& acMessage) noexcept
{
    auto& modSystem = m_world.GetModSystem();

    const uint32_t cellId = modSystem.GetGameId(acMessage.CellId);
    TESObjectCELL* pCell = RTTI_CAST(TESForm::GetById(cellId), TESForm, TESObjectCELL);
    if (!pCell)
    {
        spdlog::error("Failed to fetch cell to teleport to.");
        return;
    }

    PlayerCharacter::Get()->MoveTo(pCell, acMessage.Position);
}
