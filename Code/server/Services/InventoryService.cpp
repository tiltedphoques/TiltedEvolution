#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Events/UpdateEvent.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestCharacterInventoryChanges.h>
#include <Messages/NotifyCharacterInventoryChanges.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_objectInventoryConnection = aDispatcher.sink<PacketEvent<RequestObjectInventoryChanges>>().connect<&InventoryService::OnObjectInventoryChanges>(this);
    m_characterInventoryConnection = aDispatcher.sink<PacketEvent<RequestCharacterInventoryChanges>>().connect<&InventoryService::OnCharacterInventoryChanges>(this);
}

void InventoryService::OnUpdate(const UpdateEvent&) noexcept
{
    ProcessObjectInventoryChanges();
    ProcessCharacterInventoryChanges();
}

void InventoryService::OnObjectInventoryChanges(const PacketEvent<RequestObjectInventoryChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    for (auto& it : message.Changes)
    {
        auto view = m_world.view<FormIdComponent, ObjectComponent>();

        auto formIdIt = std::find_if(std::begin(view), std::end(view), [view, id = it.first](auto entity) {
            const auto& formIdComponent = view.get<FormIdComponent>(entity);
            return formIdComponent.Id == id;
        });

        if (formIdIt == std::end(view))
        {
            const auto entity = m_world.create();
            m_world.emplace<FormIdComponent>(entity, it.first.BaseId, it.first.ModId);
            m_world.emplace<ObjectComponent>(entity, acMessage.ConnectionId);
            m_world.emplace<CellIdComponent>(entity, it.second.CellId, it.second.WorldSpaceId, it.second.CurrentCoords);

            auto& inventoryComponent = m_world.emplace<InventoryComponent>(entity);
            inventoryComponent.Content = it.second.CurrentInventory;
            inventoryComponent.DirtyInventory = true;

            spdlog::warn("Inventory size: {} at {:x} {:x} ({}, {})", inventoryComponent.Content.Buffer.size(), it.second.WorldSpaceId.BaseId, it.second.CellId.BaseId, it.second.CurrentCoords.X, it.second.CurrentCoords.Y);
        }
        else
        {
            auto& objectComponent = m_world.get<ObjectComponent>(*formIdIt);
            objectComponent.LastSender = acMessage.ConnectionId;

            auto& inventoryComponent = m_world.get<InventoryComponent>(*formIdIt);
            inventoryComponent.Content = it.second.CurrentInventory;
            inventoryComponent.DirtyInventory = true;

            spdlog::warn("Inventory size: {} at {:x} {:x} ({}, {})", inventoryComponent.Content.Buffer.size(), it.second.WorldSpaceId.BaseId, it.second.CellId.BaseId, it.second.CurrentCoords.X, it.second.CurrentCoords.Y);
        }
    }

    spdlog::info("Inventory object change cached");
}

void InventoryService::OnCharacterInventoryChanges(const PacketEvent<RequestCharacterInventoryChanges>& acMessage) noexcept
{
    auto view = m_world.view<CharacterComponent, InventoryComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    for (auto& [id, inventory] : message.Changes)
    {
        auto itor = view.find(static_cast<entt::entity>(id));

        if (itor == std::end(view) || view.get<OwnerComponent>(*itor).ConnectionId != acMessage.ConnectionId)
            continue;

        spdlog::critical("Character inventory size: {}", inventory.Buffer.size());

        auto& inventoryComponent = view.get<InventoryComponent>(*itor);
        inventoryComponent.Content = inventory;
        inventoryComponent.DirtyInventory = true;
    }
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
    const auto objectView = m_world.view<FormIdComponent, ObjectComponent, InventoryComponent, CellIdComponent>();

    Map<ConnectionId_t, NotifyObjectInventoryChanges> messages;

    for (auto entity : objectView)
    {
        auto& formIdComponent = objectView.get<FormIdComponent>(entity);
        auto& inventoryComponent = objectView.get<InventoryComponent>(entity);
        auto& cellIdComponent = objectView.get<CellIdComponent>(entity);
        auto& objectComponent = objectView.get<ObjectComponent>(entity);

        // If we have nothing new to send skip this
        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerComponent.ConnectionId == objectComponent.LastSender)
                continue;

            const auto& playerCellIdComponent = playerView.get<CellIdComponent>(player);
            if (cellIdComponent.WorldSpaceId == GameId{})
            {
                if (playerCellIdComponent != cellIdComponent)
                {
                    continue;
                }
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

        spdlog::info("Sent inventory contents");
    }
}

void InventoryService::ProcessCharacterInventoryChanges() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 4;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto characterView = m_world.view<CharacterComponent, CellIdComponent, InventoryComponent, OwnerComponent>();

    Map<ConnectionId_t, NotifyCharacterInventoryChanges> messages;

    for (auto entity : characterView)
    {
        auto& inventoryComponent = characterView.get<InventoryComponent>(entity);
        auto& cellIdComponent = characterView.get<CellIdComponent>(entity);
        auto& ownerComponent = characterView.get<OwnerComponent>(entity);

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
            auto& change = message.Changes[World::ToInteger(entity)];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [connectionId, message] : messages)
    {
        if (!message.Changes.empty())
            GameServer::Get()->Send(connectionId, message);

        spdlog::error("Character inventory sent");
    }
}
