#pragma once

#include <Events/PacketEvent.h>
#include <Structs/GameId.h>
#include <Structs/Inventory.h>

struct World;
struct UpdateEvent;
struct RequestObjectInventoryChanges;
struct UpdateEvent;

using TiltedPhoques::Map;

class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

    void OnUpdate(const UpdateEvent&) noexcept;
    void OnObjectInventoryChanges(const PacketEvent<RequestObjectInventoryChanges>& acMessage) noexcept;
    void ProcessObjectInventoryChanges() noexcept;

private:

    World& m_world;

    Map<GameId, Inventory> m_pendingObjectInventoryChanges;

    entt::scoped_connection m_objectInventoryConnection;
};
