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
            m_world.emplace<ObjectComponent>(entity, acMessage.pPlayer);
            m_world.emplace<CellIdComponent>(entity, it.second.CellId, it.second.WorldSpaceId, it.second.CurrentCoords);

            auto& inventoryComponent = m_world.emplace<InventoryComponent>(entity);
            inventoryComponent.Content = it.second.CurrentInventory;
            inventoryComponent.DirtyInventory = true;

            spdlog::warn("Inventory size: {} at {:x} {:x} ({}, {})", inventoryComponent.Content.Buffer.size(), it.second.WorldSpaceId.BaseId, it.second.CellId.BaseId, it.second.CurrentCoords.X, it.second.CurrentCoords.Y);
        }
        else
        {
            auto& objectComponent = m_world.get<ObjectComponent>(*formIdIt);
            objectComponent.pLastSender = acMessage.pPlayer;

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
    // TODO: change this to new OwnerView
    auto view = m_world.view<CharacterComponent, InventoryComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    for (auto& [id, inventory] : message.Changes)
    {
        auto iter = view.find(static_cast<entt::entity>(id));

        if (iter == std::end(view))
            continue;

        spdlog::critical("Character inventory size: {}", inventory.Buffer.size());

        auto& inventoryComponent = view.get<InventoryComponent>(*iter);
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

    const auto objectView = m_world.view<FormIdComponent, ObjectComponent, InventoryComponent, CellIdComponent>();

    Map<Player*, NotifyObjectInventoryChanges> messages;

    for (auto entity : objectView)
    {
        auto& formIdComponent = objectView.get<FormIdComponent>(entity);
        auto& inventoryComponent = objectView.get<InventoryComponent>(entity);
        auto& cellIdComponent = objectView.get<CellIdComponent>(entity);
        auto& objectComponent = objectView.get<ObjectComponent>(entity);

        // If we have nothing new to send skip this
        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (pPlayer == objectComponent.pLastSender)
                continue;

            const auto& playerCellIdComponent = pPlayer->GetCellComponent();
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
                    !GridCellCoords::IsCellInGridCell(cellIdComponent.CenterCoords,
                                                      playerCellIdComponent.CenterCoords))
                {
                    continue;
                }
            }

            auto& message = messages[pPlayer];
            auto& change = message.Changes[formIdComponent.Id];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [pPlayer, message] : messages)
    {
        if (!message.Changes.empty())
            pPlayer->Send(message);

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

    const auto characterView = m_world.view<CharacterComponent, CellIdComponent, InventoryComponent, OwnerComponent>();

    Map<Player*, NotifyCharacterInventoryChanges> messages;

    for (auto entity : characterView)
    {
        auto& inventoryComponent = characterView.get<InventoryComponent>(entity);
        auto& cellIdComponent = characterView.get<CellIdComponent>(entity);
        auto& ownerComponent = characterView.get<OwnerComponent>(entity);

        // If we have nothing new to send skip this
        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (pPlayer == ownerComponent.GetOwner())
                continue;

            const auto& playerCellIdComponent = pPlayer->GetCellComponent();
            if (cellIdComponent.WorldSpaceId == GameId{})
            {
                if (playerCellIdComponent != cellIdComponent)
                    continue;
            }
            else
            {
                if (cellIdComponent.WorldSpaceId != playerCellIdComponent.WorldSpaceId ||
                    !GridCellCoords::IsCellInGridCell(cellIdComponent.CenterCoords,
                                                      playerCellIdComponent.CenterCoords))
                {
                    continue;
                }
            }

            auto& message = messages[pPlayer];
            auto& change = message.Changes[World::ToInteger(entity)];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [pPlayer, message] : messages)
    {
        if (!message.Changes.empty())
            pPlayer->Send(message);

        spdlog::error("Character inventory sent");
    }
}
