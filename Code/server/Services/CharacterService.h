#pragma once

#include <Events/PacketEvent.h>

struct UpdateEvent;
struct CharacterInteriorCellChangeEvent;
struct CharacterSpawnedEvent;
struct World;
struct AssignCharacterRequest;
struct CharacterSpawnRequest;
struct ClientReferencesMoveRequest;
struct RequestInventoryChanges;
struct RequestFactionsChanges;
struct RequestSpawnData;
struct GridCellCoords;
struct RequestOwnershipTransfer;
struct CharacterRemoveEvent;
struct CharacterExteriorCellChangeEvent;
struct RequestOwnershipClaim;
struct OwnershipTransferEvent;

struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    static void Serialize(const World& aRegistry, entt::entity aEntity, CharacterSpawnRequest* apSpawnRequest) noexcept;

protected:

    void OnUpdate(const UpdateEvent& acEvent) const noexcept;
    void OnCharacterExteriorCellChange(const CharacterExteriorCellChangeEvent& acEvent) const noexcept;
    void OnCharacterInteriorCellChange(const CharacterInteriorCellChangeEvent& acEvent) const noexcept;
    void OnAssignCharacterRequest(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept;
    void OnOwnershipTransferRequest(const PacketEvent<RequestOwnershipTransfer>& acMessage) const noexcept;
    void OnOwnershipTransferEvent(const OwnershipTransferEvent& acEvent) const noexcept;
    void OnOwnershipClaimRequest(const PacketEvent<RequestOwnershipClaim>& acMessage) const noexcept;
    void OnCharacterRemoveEvent(const CharacterRemoveEvent& acEvent) const noexcept;
    void OnCharacterSpawned(const CharacterSpawnedEvent& acEvent) const noexcept;
    void OnReferencesMoveRequest(const PacketEvent<ClientReferencesMoveRequest>& acMessage) const noexcept;
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) const noexcept;
    void OnFactionsChanges(const PacketEvent<RequestFactionsChanges>& acMessage) const noexcept;
    void OnRequestSpawnData(const PacketEvent<RequestSpawnData>& acMessage) const noexcept;

    void CreateCharacter(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept;

    void ProcessInventoryChanges() const noexcept;
    void ProcessFactionsChanges() const noexcept;
    void ProcessMovementChanges() const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_exteriorCellChangeEventConnection;
    entt::scoped_connection m_interiorCellChangeEventConnection;
    entt::scoped_connection m_characterAssignRequestConnection;
    entt::scoped_connection m_removeCharacterConnection;
    entt::scoped_connection m_characterSpawnedConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
    entt::scoped_connection m_inventoryChangesConnection;
    entt::scoped_connection m_factionsChangesConnection;
    entt::scoped_connection m_spawnDataConnection;
    entt::scoped_connection m_transferOwnershipConnection;
    entt::scoped_connection m_ownershipTransferEventConnection;
    entt::scoped_connection m_claimOwnershipConnection;
};
