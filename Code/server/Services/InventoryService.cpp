#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_objectInventoryConnection = aDispatcher.sink<PacketEvent<RequestObjectInventoryChanges>>().connect<&InventoryService::OnObjectInventoryChanges>(this);
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
        m_pendingObjectInventoryChanges[it.first] = it.second;
    }

    spdlog::info("Inventory object change cached");

    /*
    auto view = m_world.view<InventoryComponent>();

    for (auto& [id, inventory] : message.Changes)
    {
        auto itor = view.find(static_cast<entt::entity>(id));

        if (itor == std::end(view))
            continue;

        auto& inventoryComponent = view.get<InventoryComponent>(*itor);
        inventoryComponent.Content = inventory;
        inventoryComponent.DirtyInventory = true;
    }
    */
}

void InventoryService::ProcessObjectInventoryChanges() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 4;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_pendingObjectInventoryChanges.empty())
    {
        NotifyObjectInventoryChanges message;
        for (auto& [id, inventory] : m_pendingObjectInventoryChanges)
        {
            message.Changes[id] = inventory;
        }

        m_pendingObjectInventoryChanges.clear();

        const auto playerView = m_world.view<PlayerComponent>();
        for (const auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);
            GameServer::Get()->Send(playerComponent.ConnectionId, message);
        }

        spdlog::warn("Pending objects inventories sent");
    }
}
