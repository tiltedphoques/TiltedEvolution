#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct RequestObjectInventoryChanges;
struct RequestInventoryChanges;
struct RequestEquipmentChanges;
struct DrawWeaponRequest;
struct PlayerLeaveCellEvent;

/**
* @brief Relays inventory/equipment changes and updates the server side state.
*/
class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

    /**
    * @brief Relays inventory changes to other clients and updates server side inventories.
    */
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept;
    /**
    * @brief Relays equipment changes to other clients and updates server side equipment.
    */
    void OnEquipmentChanges(const PacketEvent<RequestEquipmentChanges>& acMessage) noexcept;
    /**
    * @brief Relays weapon draw changes to other clients and updates server side weapon draw state.
    */
    void OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept;

private:

    World& m_world;

    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
    entt::scoped_connection m_drawWeaponConnection;
};
