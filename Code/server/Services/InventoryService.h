#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct RequestObjectInventoryChanges;
struct RequestInventoryChanges;
struct DrawWeaponRequest;
struct UpdateEvent;
struct PlayerLeaveCellEvent;

class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

    void OnObjectInventoryChanges(const PacketEvent<RequestObjectInventoryChanges>& acMessage) noexcept;
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept;
    void OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept;

private:

    World& m_world;

    entt::scoped_connection m_objectInventoryConnection;
    entt::scoped_connection m_characterInventoryConnection;
    entt::scoped_connection m_drawWeaponConnection;
};
