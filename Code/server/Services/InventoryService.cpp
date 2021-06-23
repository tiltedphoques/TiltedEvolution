#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Events/PlayerLeaveCellEvent.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_objectInventoryConnection = aDispatcher.sink<PacketEvent<RequestObjectInventoryChanges>>().connect<&InventoryService::OnObjectInventoryChanges>(this);
    m_leaveCellConnection = aDispatcher.sink<PlayerLeaveCellEvent>().connect<&InventoryService::OnPlayerLeaveCellEvent>(this);
}

void InventoryService::OnUpdate(const UpdateEvent&) noexcept
{
    ProcessObjectInventoryChanges();
}

void InventoryService::OnObjectInventoryChanges(const PacketEvent<RequestObjectInventoryChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    for (auto& it : message.Changes)
    {
        auto view = m_world.view<FormIdComponent>();

        auto formIdIt = std::find_if(std::begin(view), std::end(view), [view, id = it.first](auto entity) {
            const auto& formIdComponent = view.get<FormIdComponent>(entity);
            return formIdComponent.Id == id;
        });

        InventoryComponent inventoryComponent;

        if (formIdIt == std::end(view))
        {
            const auto entity = m_world.create();
            m_world.emplace<FormIdComponent>(entity, it.first.BaseId, it.first.ModId);
            m_world.emplace<ObjectComponent>(entity, acMessage.ConnectionId);
            m_world.emplace<CellIdComponent>(entity, it.second.CellId, it.second.WorldSpaceId, it.second.CurrentCoords);
            inventoryComponent = m_world.emplace<InventoryComponent>(entity);
        }
        else
        {
            auto& objectComponent = m_world.get<ObjectComponent>(*formIdIt);
            objectComponent.LastSender = acMessage.ConnectionId;
            inventoryComponent = m_world.get<InventoryComponent>(*formIdIt);
        }

        inventoryComponent.Content = it.second.CurrentInventory;
        inventoryComponent.DirtyInventory = true;
        spdlog::warn("Inventory size: {}", inventoryComponent.Content.Buffer.size());
    }

    spdlog::info("Inventory object change cached");
}

void InventoryService::ProcessObjectInventoryChanges() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 4;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto objectView = m_world.view<FormIdComponent, ObjectComponent, InventoryComponent, CellIdComponent, OwnerComponent>();

    Map<ConnectionId_t, NotifyObjectInventoryChanges> messages;

    for (auto entity : objectView)
    {
        auto& formIdComponent = objectView.get<FormIdComponent>(entity);
        auto& inventoryComponent = objectView.get<InventoryComponent>(entity);
        auto& cellIdComponent = objectView.get<CellIdComponent>(entity);
        auto& ownerComponent = objectView.get<OwnerComponent>(entity);

        // If we have nothing new to send skip this
        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerComponent.ConnectionId == ownerComponent.ConnectionId)
                continue;

            const auto& playerCellIdComponent = playerView.get<CellIdComponent>(player);
            if (cellIdComponent.WorldSpaceId == GameId{})
            {
                if (playerCellIdComponent != cellIdComponent)
                    continue;
            }
            else
            {
                if (cellIdComponent.WorldSpaceId != playerCellIdComponent.WorldSpaceId ||
                    !GridCellCoords::IsCellInGridCell(&cellIdComponent.CenterCoords,
                                                      &playerCellIdComponent.CenterCoords))
                {
                    continue;
                }
            }

            auto& message = messages[playerComponent.ConnectionId];
            auto& change = message.Changes[formIdComponent.Id];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [connectionId, message] : messages)
    {
        if (!message.Changes.empty())
            GameServer::Get()->Send(connectionId, message);
    }
}
