#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct RequestInventoryChanges;

class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

private:
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept;

    World& m_world;

    entt::scoped_connection m_inventoryConnection;
};
