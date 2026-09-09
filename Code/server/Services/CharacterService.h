#pragma once

#include <Events/PacketEvent.h>
#include <Structs/ActorData.h>

struct UpdateEvent;
struct CharacterInteriorCellChangeEvent;
struct CharacterSpawnedEvent;
struct World;
struct AssignCharacterRequest;
struct AssignCharacterResponse;
struct CharacterSpawnRequest;
struct ClientReferencesMoveRequest;
struct RequestFactionsChanges;
struct GridCellCoords;
struct RequestOwnershipTransfer;
struct CharacterRemoveEvent;
struct CharacterExteriorCellChangeEvent;
struct RequestOwnershipClaim;
struct OwnershipTransferEvent;
struct MountRequest;
struct NewPackageRequest;
struct RequestRespawn;
struct SyncExperienceRequest;
struct DialogueRequest;
struct SubtitleRequest;
struct Player;

/**
 * @brief Manages player and actor state.
 */
struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    static void Serialize(World& aRegistry, entt::entity aEntity, CharacterSpawnRequest* apSpawnRequest) noexcept;

protected:
    enum class OwnershipTransferReason : uint8_t
    {
        LeaderAssignment,
        LeaderClaim,
        Mount,
        Relinquish,
        OwnerUnavailable
    };

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
    void OnFactionsChanges(const PacketEvent<RequestFactionsChanges>& acMessage) const noexcept;
    void OnMountRequest(const PacketEvent<MountRequest>& acMessage) const noexcept;
    void OnNewPackageRequest(const PacketEvent<NewPackageRequest>& acMessage) const noexcept;
    void OnRequestRespawn(const PacketEvent<RequestRespawn>& acMessage) const noexcept;
    void OnSyncExperienceRequest(const PacketEvent<SyncExperienceRequest>& acMessage) const noexcept;
    void OnDialogueRequest(const PacketEvent<DialogueRequest>& acMessage) const noexcept;
    void OnSubtitleRequest(const PacketEvent<SubtitleRequest>& acMessage) const noexcept;

    void CreateCharacter(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept;
    void PopulateAssignmentResponse(entt::entity aEntity, AssignCharacterResponse& aResponse) const noexcept;
    static const char* GetOwnershipTransferReasonName(OwnershipTransferReason aReason) noexcept;
    bool CanClaimOwnership(Player* apPlayer, entt::entity aEntity, uint32_t aExpectedOwnershipEpoch, OwnershipTransferReason aReason) const noexcept;
    bool TransferOwnership(Player* apPlayer, entt::entity aEntity, OwnershipTransferReason aReason, bool aResetInvalidOwners = true) const noexcept;
    void TransferToNextOwner(entt::entity aEntity, OwnershipTransferReason aReason) const noexcept;
    ActorData BuildActorData(const entt::entity acEntity) const noexcept;

    void ProcessFactionsChanges() const noexcept;
    void ProcessMovementChanges() const noexcept;

private:
    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_exteriorCellChangeEventConnection;
    entt::scoped_connection m_interiorCellChangeEventConnection;
    entt::scoped_connection m_characterAssignRequestConnection;
    entt::scoped_connection m_transferOwnershipConnection;
    entt::scoped_connection m_ownershipTransferEventConnection;
    entt::scoped_connection m_claimOwnershipConnection;
    entt::scoped_connection m_removeCharacterConnection;
    entt::scoped_connection m_characterSpawnedConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
    entt::scoped_connection m_factionsChangesConnection;
    entt::scoped_connection m_mountConnection;
    entt::scoped_connection m_newPackageConnection;
    entt::scoped_connection m_requestRespawnConnection;
    entt::scoped_connection m_syncExperienceConnection;
    entt::scoped_connection m_dialogueConnection;
    entt::scoped_connection m_subtitleConnection;
};
