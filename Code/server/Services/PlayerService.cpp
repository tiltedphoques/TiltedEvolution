#include "Events/CharacterInteriorCellChangeEvent.h"
#include "Events/CharacterExteriorCellChangeEvent.h"
#include "Events/PlayerLeaveCellEvent.h"

#include <Services/PlayerService.h>
#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>
#include <console/ConsoleRegistry.h>

#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/PlayerRespawnRequest.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/NotifyPlayerRespawn.h>

Console::Setting fGoldLossFactor{"Gameplay:fGoldLossFactor", "Factor of the amount of gold lost on death", 0.05f};

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_interiorCellEnterConnection(aDispatcher.sink<PacketEvent<EnterInteriorCellRequest>>().connect<&PlayerService::HandleInteriorCellEnter>(this))
    , m_gridCellShiftConnection(aDispatcher.sink<PacketEvent<ShiftGridCellRequest>>().connect<&PlayerService::HandleGridCellShift>(this))
    , m_exteriorCellEnterConnection(aDispatcher.sink<PacketEvent<EnterExteriorCellRequest>>().connect<&PlayerService::HandleExteriorCellEnter>(this))
    , m_playerRespawnConnection(aDispatcher.sink<PacketEvent<PlayerRespawnRequest>>().connect<&PlayerService::OnPlayerRespawnRequest>(this))
{
}

void PlayerService::HandleGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept
{
    auto* pPlayer = acMessage.pPlayer;

    auto& message = acMessage.Packet;

    const GameId oldCell = pPlayer->GetCellComponent().Cell;

    CellIdComponent cell = CellIdComponent{message.PlayerCell, message.WorldSpaceId, message.CenterCoords};
    pPlayer->SetCellComponent(cell);

    m_world.GetDispatcher().trigger(PlayerLeaveCellEvent(oldCell));

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);
        const auto& characterCellComponent = characterView.get<CellIdComponent>(character);

        if (ownedComponent.GetOwner() == pPlayer)
            continue;

        const auto cellIt = std::find_if(std::begin(message.Cells), std::end(message.Cells),
            [Cells = message.Cells, CharacterCell = characterCellComponent.Cell](auto playerCell)
        {
           return playerCell == CharacterCell;
        });

        if (cellIt == std::end(message.Cells))
        {
            continue;
        }

        CharacterSpawnRequest spawnMessage;
        CharacterService::Serialize(m_world, character, &spawnMessage);

        pPlayer->Send(spawnMessage);
    }
}

void PlayerService::HandleExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;
    auto* pPlayer = acMessage.pPlayer;

    if (pPlayer->GetCharacter())
    {
        auto entity = *pPlayer->GetCharacter();

        auto cell = CellIdComponent{message.CellId, message.WorldSpaceId, message.CurrentCoords};

        if (pPlayer->GetCellComponent())
        {
            m_world.GetDispatcher().trigger(CharacterExteriorCellChangeEvent{pPlayer, entity, message.WorldSpaceId, message.CurrentCoords});
        }

        pPlayer->SetCellComponent(cell);
    }
}

void PlayerService::HandleInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept
{
    auto* pPlayer = acMessage.pPlayer;

    auto& message = acMessage.Packet;

    const auto oldCell = pPlayer->GetCellComponent().Cell;

    auto cell = CellIdComponent{message.CellId, {}, {}};
    pPlayer->SetCellComponent(cell);

    m_world.GetDispatcher().trigger(PlayerLeaveCellEvent(oldCell));

    if (pPlayer->GetCharacter())
    {
        auto entity = *pPlayer->GetCharacter();

        if (auto pCellIdComponent = m_world.try_get<CellIdComponent>(entity); pCellIdComponent)
        {
            m_world.GetDispatcher().trigger(CharacterInteriorCellChangeEvent{pPlayer, entity, message.CellId});
        }
    }

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);

        if (ownedComponent.GetOwner() == pPlayer)
            continue;

        if (message.CellId != characterView.get<CellIdComponent>(character).Cell)
            continue;

        CharacterSpawnRequest spawnMessage;
        CharacterService::Serialize(m_world, character, &spawnMessage);

        pPlayer->Send(spawnMessage);
    }
}

void PlayerService::OnPlayerRespawnRequest(const PacketEvent<PlayerRespawnRequest>& acMessage) const noexcept
{
    float goldLossFactor = fGoldLossFactor.as_float();

    if (goldLossFactor == 0.0)
        return;

    auto character = acMessage.pPlayer->GetCharacter();
    if (!character)
        return;

    auto view = m_world.view<InventoryComponent>();

    const auto it = view.find(static_cast<entt::entity>(*character));

    if (it != view.end())
    {
        auto& inventoryComponent = view.get<InventoryComponent>(*it);

        GameId goldId(0, 0xF);
        int32_t goldCount = inventoryComponent.Content.GetEntryCountById(goldId);
        int32_t goldToRemove = goldCount * goldLossFactor;

        Inventory::Entry entry{};
        entry.BaseId = goldId;
        entry.Count = -goldToRemove;

        inventoryComponent.Content.AddOrRemoveEntry(entry);

        NotifyInventoryChanges notify{};
        notify.ServerId = World::ToInteger(*character);
        notify.Item = entry;
        notify.Drop = false;

        // Exclude respawned player from inventory changes notification...
        GameServer::Get()->SendToPlayersInRange(notify, *character, acMessage.GetSender());

        // ...and instead, send NotifyPlayerRespawn so that the client can print a message.
        NotifyPlayerRespawn notifyRespawn{};
        notifyRespawn.GoldLost = goldToRemove;

        acMessage.pPlayer->Send(notifyRespawn);
    }
}
