#include "InventoryService.h"

#include <Components.h>
#include <World.h>

#include <Messages/RequestInventoryChanges.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_inventoryConnection = aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&InventoryService::OnInventoryChanges>(this);
}

void InventoryService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept
{
    auto view = m_world.view<InventoryComponent>();

    auto& message = acMessage.Packet;

    for (auto& [id, inventory] : message.Changes)
    {
        auto itor = view.find(static_cast<entt::entity>(id));

        if (itor == std::end(view))
            continue;

        auto& inventoryComponent = view.get<InventoryComponent>(*itor);
        inventoryComponent.Content = inventory;
        inventoryComponent.DirtyInventory = true;
    }
}
