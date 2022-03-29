#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct RequestObjectInventoryChanges;
struct RequestInventoryChanges;
struct RequestEquipmentChanges;
struct DrawWeaponRequest;
struct PlayerLeaveCellEvent;

class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept;
    void OnEquipmentChanges(const PacketEvent<RequestEquipmentChanges>& acMessage) noexcept;
    void OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept;

private:

    World& m_world;

    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
    entt::scoped_connection m_drawWeaponConnection;
};
