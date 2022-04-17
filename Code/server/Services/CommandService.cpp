#include <Services/CommandService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Messages/TeleportCommandRequest.h>
#include <Messages/TeleportCommandResponse.h>

CommandService::CommandService(World& aWorld, entt::dispatcher& aDispatcher) noexcept 
    : m_world(aWorld)
{
    m_teleportConnection = aDispatcher.sink<PacketEvent<TeleportCommandRequest>>().connect<&CommandService::OnTeleportCommandRequest>(this);
}

void CommandService::OnTeleportCommandRequest(const PacketEvent<TeleportCommandRequest>& acMessage) const noexcept
{
    Player* pTargetPlayer = nullptr;
    for (Player* pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer->GetUsername() == acMessage.Packet.TargetPlayer)
            pTargetPlayer = pPlayer;
    }

    TeleportCommandResponse response{};
    if (pTargetPlayer)
    {
        auto character = pTargetPlayer->GetCharacter();
        if (character)
        {
            const auto* pMovementComponent = m_world.try_get<MovementComponent>(*character);
            if (pMovementComponent)
            {
                response.CellId = pTargetPlayer->GetCellComponent().Cell;
                response.Position = pMovementComponent->Position;
            }
        }
    }

    spdlog::info("TeleportCommandResponse: {:X}:{:X} at position {}, {}, {}", response.CellId.ModId, response.CellId.BaseId, response.Position.x, response.Position.y, response.Position.z);

    acMessage.pPlayer->Send(response);
}
