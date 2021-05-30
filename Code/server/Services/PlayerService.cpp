#include <stdafx.h>

#include "Events/CharacterInteriorCellChangeEvent.h"
#include "Events/CharacterExteriorCellChangeEvent.h"

#include <Services/PlayerService.h>
#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>

#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/CharacterSpawnRequest.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_interiorCellEnterConnection(aDispatcher.sink<PacketEvent<EnterInteriorCellRequest>>().connect<&PlayerService::HandleInteriorCellEnter>(this))
    , m_gridCellShiftConnection(aDispatcher.sink<PacketEvent<ShiftGridCellRequest>>().connect<&PlayerService::HandleGridCellShift>(this))
    , m_exteriorCellEnterConnection(aDispatcher.sink<PacketEvent<EnterExteriorCellRequest>>().connect<&PlayerService::HandleExteriorCellEnter>(this))
{
}

void PlayerService::HandleGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    m_world.emplace_or_replace<CellIdComponent>(acMessage.Entity, message.PlayerCell, message.WorldSpaceId, message.CenterCoords);

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);
        const auto& characterCellComponent = characterView.get<CellIdComponent>(character);

        if (ownedComponent.ConnectionId == acMessage.PlayerComponent.ConnectionId)
            continue;

        const auto cellItor = std::find_if(std::begin(message.Cells), std::end(message.Cells),
            [Cells = message.Cells, CharacterCell = characterCellComponent.Cell](auto playerCell)
        {
           return playerCell == CharacterCell;
        });

        if (cellItor == std::end(message.Cells))
            continue;

        CharacterSpawnRequest spawnMessage;
        CharacterService::Serialize(m_world, character, &spawnMessage);

        GameServer::Get()->Send(acMessage.PlayerComponent.ConnectionId, spawnMessage);
    }
}

void PlayerService::HandleExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto& playerComponent = acMessage.PlayerComponent;

    if (playerComponent.Character)
    {
        if (auto pCellIdComponent = m_world.try_get<CellIdComponent>(*playerComponent.Character); pCellIdComponent)
        {
            m_world.GetDispatcher().trigger(CharacterExteriorCellChangeEvent{acMessage.Entity, *playerComponent.Character,
                                                                             message.WorldSpaceId, message.CurrentCoords});

            pCellIdComponent->Cell = message.CellId;
            pCellIdComponent->WorldSpaceId = message.WorldSpaceId;
            pCellIdComponent->CenterCoords = message.CurrentCoords;
        }
        else
            m_world.emplace<CellIdComponent>(*playerComponent.Character, message.CellId, message.WorldSpaceId, message.CurrentCoords);
    }
}

void PlayerService::HandleInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    m_world.emplace_or_replace<CellIdComponent>(acMessage.Entity, message.CellId);

    auto& playerComponent = acMessage.PlayerComponent;

    if (playerComponent.Character)
    {
        if (auto pCellIdComponent = m_world.try_get<CellIdComponent>(*playerComponent.Character); pCellIdComponent)
        {
            m_world.GetDispatcher().trigger(CharacterInteriorCellChangeEvent{acMessage.Entity, *playerComponent.Character, message.CellId});

            pCellIdComponent->Cell = message.CellId;
            pCellIdComponent->WorldSpaceId = GameId{};
            pCellIdComponent->CenterCoords = GridCellCoords{};
        }
        else
            m_world.emplace<CellIdComponent>(*playerComponent.Character, message.CellId);
    }

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);

        if (ownedComponent.ConnectionId == acMessage.PlayerComponent.ConnectionId)
            continue;

        if (message.CellId != characterView.get<CellIdComponent>(character).Cell)
            continue;

        CharacterSpawnRequest spawnMessage;
        CharacterService::Serialize(m_world, character, &spawnMessage);

        spdlog::critical("Sending interior character {:x}", spawnMessage.ServerId);
        GameServer::Get()->Send(acMessage.PlayerComponent.ConnectionId, spawnMessage);
    }
}
