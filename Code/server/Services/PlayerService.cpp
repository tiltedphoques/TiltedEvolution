#include <stdafx.h>

#include "Events/CharacterCellChangeEvent.h"
#include "Events/CharacterGridCellShiftEvent.h"

#include <Services/PlayerService.h>
#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>

#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterCellRequest.h>
#include <Messages/CharacterSpawnRequest.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_cellEnterConnection(aDispatcher.sink<PacketEvent<EnterCellRequest>>().connect<&PlayerService::HandleCellEnter>(this)),
      m_gridCellShiftConnection(aDispatcher.sink<PacketEvent<ShiftGridCellRequest>>().connect<&PlayerService::HandleGridCellShift>(this))
{
}

void PlayerService::HandleGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept
{
    auto playerView = m_world.view<PlayerComponent>();

    const auto itor = std::find_if(std::begin(playerView), std::end(playerView),
        [playerView, connectionId = acMessage.ConnectionId](auto entity)
    {
        const auto& [playerComponent] = playerView.get(entity);
        return playerComponent.ConnectionId == connectionId;
    });

    if(itor == std::end(playerView))
    {
        spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
        return;
    }

    auto& message = acMessage.Packet;

    m_world.emplace_or_replace<CellIdComponent>(*itor, message.PlayerCell, message.WorldSpaceId);

    auto& playerComponent = playerView.get<PlayerComponent>(*itor);

    if (playerComponent.Character)
    {
        if (auto pCellIdComponent = m_world.try_get<CellIdComponent>(*playerComponent.Character); pCellIdComponent)
        {
            auto characterComponent = m_world.get<CharacterComponent>(*playerComponent.Character);

            m_world.GetDispatcher().trigger(CharacterGridCellShiftEvent{*itor, *playerComponent.Character, pCellIdComponent->WorldSpaceId, 
                                                                        message.WorldSpaceId, characterComponent.CenterCoords, message.CenterCoords});

            pCellIdComponent->Cell = message.PlayerCell;
            pCellIdComponent->WorldSpaceId = message.WorldSpaceId;
            characterComponent.CenterCoords = message.CenterCoords;
        }
        else
            m_world.emplace<CellIdComponent>(*playerComponent.Character, message.PlayerCell, message.WorldSpaceId);
    }

    for (auto cell : message.Cells)
    {
        auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
        for (auto character : characterView)
        {
            const auto& ownedComponent = characterView.get<OwnerComponent>(character);
            const auto& characterComponent = characterView.get<CharacterComponent>(character);
            const auto& characterCellComponent = characterView.get<CellIdComponent>(character);

            // Don't send self managed
            if (ownedComponent.ConnectionId == acMessage.ConnectionId)
                continue;

            if (cell != characterCellComponent.Cell)
                continue;

            CharacterSpawnRequest spawnMessage;
            CharacterService::Serialize(m_world, character, &spawnMessage);

            GameServer::Get()->Send(acMessage.ConnectionId, spawnMessage);
            spdlog::error("CharacterSpawnRequest {:x}", spawnMessage.FormId.BaseId);
        }
    }
}

void PlayerService::HandleCellEnter(const PacketEvent<EnterCellRequest>& acMessage) const noexcept
{
    auto playerView = m_world.view<PlayerComponent>();

    const auto itor = std::find_if(std::begin(playerView), std::end(playerView),
   [playerView, connectionId = acMessage.ConnectionId](auto entity)
    {
        const auto& [playerComponent] = playerView.get(entity);
        return playerComponent.ConnectionId == connectionId;
    });

    if(itor == std::end(playerView))
    {
        spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
        return;
    }

    auto& message = acMessage.Packet;

    m_world.emplace_or_replace<CellIdComponent>(*itor, message.CellId);

    auto& playerComponent = playerView.get<PlayerComponent>(*itor);

    if (playerComponent.Character)
    {
        if (auto pCellIdComponent = m_world.try_get<CellIdComponent>(*playerComponent.Character); pCellIdComponent)
        {
            m_world.GetDispatcher().trigger(CharacterCellChangeEvent{*itor, *playerComponent.Character, pCellIdComponent->Cell, message.CellId});

            pCellIdComponent->Cell = message.CellId;
        }
        else
            m_world.emplace<CellIdComponent>(*playerComponent.Character, message.CellId);
    }

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);

        // Don't send self managed
        if (ownedComponent.ConnectionId == acMessage.ConnectionId)
            continue;

        if (message.CellId != characterView.get<CellIdComponent>(character).Cell)
            continue;

        CharacterSpawnRequest spawnMessage;
        CharacterService::Serialize(m_world, character, &spawnMessage);

        GameServer::Get()->Send(acMessage.ConnectionId, spawnMessage);
    }
}
