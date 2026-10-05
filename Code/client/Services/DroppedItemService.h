#pragma once

#include <Structs/DroppedItemData.h>

struct World;
struct TransportService;
struct TESObjectREFR;

struct UpdateEvent;
struct DisconnectedEvent;
struct DropItemEvent;
struct ReferencePickUpEvent;
struct DropItemResponse;
struct NotifyDroppedItemsSpawn;
struct NotifyDroppedItemMove;
struct NotifyDroppedItemsRemove;

/**
 * @brief Syncs dropped items: the dropper simulates and streams the transform, other clients follow with a keyframed copy.
 * The server decides which copies each client holds, so none is left untracked; the first pickup deletes the rest.
 */
struct DroppedItemService
{
    DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~DroppedItemService() noexcept = default;

    TP_NOCOPYMOVE(DroppedItemService);

protected:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnDropItem(const DropItemEvent& acEvent) noexcept;
    void OnReferencePickUp(const ReferencePickUpEvent& acEvent) noexcept;
    void OnDropItemResponse(const DropItemResponse& acMessage) noexcept;
    void OnNotifyDroppedItemsSpawn(const NotifyDroppedItemsSpawn& acMessage) noexcept;
    void OnNotifyDroppedItemMove(const NotifyDroppedItemMove& acMessage) noexcept;
    void OnNotifyDroppedItemsRemove(const NotifyDroppedItemsRemove& acMessage) noexcept;

private:
    struct TrackedItem
    {
        uint32_t FormId{};
        // Checked before touching the reference, in case the temporary form id was reused.
        uint32_t BaseFormId{};
        // Zero until the server answers the drop.
        uint32_t ServerId{};
        bool IsSimulatedLocally{};
        bool IsAtRest{};

        // Simulating client
        bool IsPickedUpBeforeRegistration{};
        double SimulationTime{};
        double RestTime{};
        double TimeSinceSend{};
        DroppedItemTransform LastTransform{};
        DroppedItemTransform LastSentTransform{};

        // Following client
        bool IsKeyframed{};
        bool IsSettling{};
        double TimeSinceUpdate{};
        DroppedItemTransform TargetTransform{};
    };

    void UpdateSimulatedItem(TrackedItem& aItem, double aDelta) noexcept;
    void UpdateFollowingItem(TrackedItem& aItem, double aDelta) noexcept;
    void SendMove(TrackedItem& aItem) noexcept;
    void SendPickUp(const TrackedItem& acItem) const noexcept;
    static void EnsureKeyframed(TrackedItem& aItem, TESObjectREFR* apReference) noexcept;
    void FinishFollowing(TrackedItem& aItem) noexcept;
    TESObjectREFR* SpawnItem(const DroppedItemData& acData) const noexcept;
    TESObjectREFR* GetReference(const TrackedItem& acItem) const noexcept;
    void DeleteReference(const TrackedItem& acItem) const noexcept;
    TrackedItem* FindByFormId(uint32_t aFormId) noexcept;
    TrackedItem* FindByServerId(uint32_t aServerId) noexcept;
    void Erase(const TrackedItem& acItem) noexcept;

    World& m_world;
    TransportService& m_transport;

    Vector<TrackedItem> m_items{};

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_dropItemConnection;
    entt::scoped_connection m_referencePickUpConnection;
    entt::scoped_connection m_dropItemResponseConnection;
    entt::scoped_connection m_droppedItemsSpawnConnection;
    entt::scoped_connection m_droppedItemMoveConnection;
    entt::scoped_connection m_droppedItemsRemoveConnection;
};
