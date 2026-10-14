#pragma once

#include <Structs/Inventory.h>
#include <Structs/DroppedItemData.h>

struct World;
struct TransportService;
struct TESObjectREFR;
struct TESObjectCELL;
struct TESBoundObject;
struct NiPoint3;

struct UpdateEvent;
struct DisconnectedEvent;
struct ItemDroppedEvent;
struct DroppedItemPickedUpEvent;
struct DropItemResponse;
struct NotifyDroppedItems;
struct NotifyDroppedItemRemoved;
struct PickupDroppedItemResponse;

/**
 * @brief Syncs items that players drop into the world.
 *
 * The server gives every dropped item an id. Each client maps that id to its own local reference,
 * places the item for players that did not drop it and removes it again once somebody picks it up.
 * Pickups go through the server, so only one player can ever get a dropped item.
 */
struct DroppedItemService
{
    DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~DroppedItemService() noexcept = default;

    TP_NOCOPYMOVE(DroppedItemService);

private:
    struct TrackedItem
    {
        uint32_t RefId{};
        uint32_t BaseFormId{};
        Inventory::Entry Item{};
    };

    struct SettlingDrop
    {
        uint32_t ActorId{};
        uint32_t BaseFormId{};
        Inventory::Entry Item{};
        double Elapsed{};
        float LastX{};
        float LastY{};
        float LastZ{};
    };

    struct PendingDrop
    {
        uint32_t BaseFormId{};
        Inventory::Entry Item{};
        // Set when the item is picked up again before the server assigned it an id.
        bool PickedUp{};
        uint32_t PickerId{};
    };

    struct PendingPickup
    {
        uint32_t ActorId{};
        Inventory::Entry Item{};
    };

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnDisconnected(const DisconnectedEvent& acEvent) noexcept;
    void OnItemDropped(const ItemDroppedEvent& acEvent) noexcept;
    void OnDroppedItemPickedUp(const DroppedItemPickedUpEvent& acEvent) noexcept;
    void OnDropItemResponse(const DropItemResponse& acMessage) noexcept;
    void OnNotifyDroppedItems(const NotifyDroppedItems& acMessage) noexcept;
    void OnNotifyDroppedItemRemoved(const NotifyDroppedItemRemoved& acMessage) noexcept;
    void OnPickupDroppedItemResponse(const PickupDroppedItemResponse& acMessage) noexcept;

    /**
     * @brief Tries to place every dropped item whose cell is loaded, the rest is retried later.
     */
    void SpawnPendingItems() noexcept;
    /**
     * @return false if the item's cell is not loaded yet.
     */
    bool SpawnItem(const DroppedItemData& acData) noexcept;
    TESObjectREFR* FindUntrackedReference(const TESObjectCELL* apCell, const TESBoundObject* apObject, const NiPoint3& acPosition) const noexcept;
    TESObjectREFR* CreateReference(TESObjectCELL* apCell, TESBoundObject* apObject, const Inventory::Entry& acItem, NiPoint3 aPosition, NiPoint3 aRotation) const noexcept;
    TESObjectREFR* GetReference(const TrackedItem& acItem) const noexcept;
    /**
     * @brief Sends drops whose item came to rest, so that other players place it where it actually lies.
     */
    void UpdateSettlingDrops(double aDelta) noexcept;
    void SendDrop(uint32_t aRefId, const SettlingDrop& acDrop) noexcept;

    bool IsTrackedReference(uint32_t aRefId) const noexcept;
    void SendPickup(uint32_t aServerId, uint32_t aActorId, Inventory::Entry aItem) noexcept;

    World& m_world;
    TransportService& m_transport;

    // Keyed by server id.
    Map<uint32_t, TrackedItem> m_items;
    // Keyed by the local reference id, dropped items still flying or rolling.
    Map<uint32_t, SettlingDrop> m_settlingDrops;
    // Keyed by the local reference id, waiting for a server id.
    Map<uint32_t, PendingDrop> m_pendingDrops;
    // Keyed by server id, items whose cell was not loaded yet.
    Map<uint32_t, DroppedItemData> m_unspawnedItems;
    // Keyed by server id, pickups waiting for the server's verdict.
    Map<uint32_t, PendingPickup> m_pendingPickups;
    // Keyed by local reference id, items removed because someone else picked them up.
    // Pickup events are handled a frame late, so a local pickup can still show up for these.
    Map<uint32_t, std::pair<uint32_t, TrackedItem>> m_removedItems;

    double m_spawnTimer{};

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_itemDroppedConnection;
    entt::scoped_connection m_pickedUpConnection;
    entt::scoped_connection m_dropItemResponseConnection;
    entt::scoped_connection m_notifyDroppedItemsConnection;
    entt::scoped_connection m_notifyDroppedItemRemovedConnection;
    entt::scoped_connection m_pickupResponseConnection;
};
