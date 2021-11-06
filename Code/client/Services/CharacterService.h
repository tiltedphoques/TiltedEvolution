#pragma once
#include "Structs/Inventory.h"

struct UpdateEvent;
struct ConnectedEvent;
struct DisconnectedEvent;
struct EquipmentChangeEvent;
struct FormIdComponent;
struct ActionEvent;
struct AssignCharacterResponse;
struct CharacterSpawnRequest;
struct ServerReferencesMoveRequest;
struct NotifyInventoryChanges;
struct NotifyFactionsChanges;
struct NotifyRemoveCharacter;
struct NotifySpawnData;
struct NotifyOwnershipTransfer;
struct SpellCastEvent;
struct NotifySpellCast;
struct InterruptCastEvent;
struct NotifyInterruptCast;
struct AddTargetEvent;
struct NotifyAddTarget;
struct ProjectileLaunchedEvent;
struct NotifyProjectileLaunch;

struct Actor;
struct World;
struct TransportService;

struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    void OnFormIdComponentAdded(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void OnFormIdComponentRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnConnected(const ConnectedEvent& acConnectedEvent) const noexcept;
    void OnDisconnected(const DisconnectedEvent& acDisconnectedEvent) const noexcept;
    void OnAssignCharacter(const AssignCharacterResponse& acMessage) const noexcept;
    void OnCharacterSpawn(const CharacterSpawnRequest& acMessage) const noexcept;
    void OnReferencesMoveRequest(const ServerReferencesMoveRequest& acMessage) const noexcept;
    void OnActionEvent(const ActionEvent& acActionEvent) const noexcept;
    void OnFactionsChanges(const NotifyFactionsChanges& acEvent) const noexcept;
    void OnOwnershipTransfer(const NotifyOwnershipTransfer& acMessage) const noexcept;
    void OnRemoveCharacter(const NotifyRemoveCharacter& acMessage) const noexcept;
    void OnRemoteSpawnDataReceived(const NotifySpawnData& acEvent) const noexcept;
    void OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept;
    void OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept;
    void OnInterruptCast(const InterruptCastEvent& acEvent) const noexcept;
    void OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept;
    void OnAddTargetEvent(const AddTargetEvent& acEvent) const noexcept;
    void OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept;
    void OnProjectileLaunchedEvent(const ProjectileLaunchedEvent& acEvent) const noexcept;
    void OnNotifyProjectileLaunch(const NotifyProjectileLaunch& acMessage) const noexcept;

private:

    void RequestServerAssignment(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void CancelServerAssignment(entt::registry& aRegistry, entt::entity aEntity, uint32_t aFormId) const noexcept;

    Actor* CreateCharacterForEntity(entt::entity aEntity) const noexcept;

    void RunLocalUpdates() const noexcept;
    void RunRemoteUpdates() const noexcept;
    void RunFactionsUpdates() const noexcept;
    void RunSpawnUpdates() const noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_formIdAddedConnection;
    entt::scoped_connection m_formIdRemovedConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_actionConnection;
    entt::scoped_connection m_factionsConnection;
    entt::scoped_connection m_ownershipTransferConnection;
    entt::scoped_connection m_removeCharacterConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_assignCharacterConnection;
    entt::scoped_connection m_characterSpawnConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
    entt::scoped_connection m_remoteSpawnDataReceivedConnection;
    entt::scoped_connection m_spellCastEventConnection;
    entt::scoped_connection m_notifySpellCastConnection;
    entt::scoped_connection m_interruptCastEventConnection;
    entt::scoped_connection m_notifyInterruptCastConnection;
    entt::scoped_connection m_addTargetEventConnection;
    entt::scoped_connection m_notifyAddTargetConnection;
    entt::scoped_connection m_projectileLaunchedConnection;
    entt::scoped_connection m_projectileLaunchConnection;
};
