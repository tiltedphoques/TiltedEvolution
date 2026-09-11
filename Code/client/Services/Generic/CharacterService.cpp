#include "Forms/TESObjectCELL.h"
#include "Forms/TESWorldSpace.h"
#include "Services/PapyrusService.h"
#include <Services/PartyService.h>

#include <Services/CharacterService.h>
#include <Services/QuestService.h>
#include <Services/TransportService.h>

#include <Games/References.h>
#include <Games/Misc/SubtitleManager.h>

#include <Forms/TESNPC.h>
#include <Interface/UI.h>
#include <Forms/TESQuest.h>

#include <BranchInfo.h>
#include <Components.h>

#include <Systems/InterpolationSystem.h>
#include <Systems/AnimationSystem.h>
#include <Systems/CacheSystem.h>
#include <Systems/FaceGenSystem.h>

#include <Events/ActorAddedEvent.h>
#include <Events/ActorRemovedEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/MountEvent.h>
#include <Events/InitPackageEvent.h>
#include <Events/BeastFormChangeEvent.h>
#include <Events/AddExperienceEvent.h>
#include <Events/DialogueEvent.h>
#include <Events/SubtitleEvent.h>
#include <Events/MoveActorEvent.h>
#include <Events/PartyJoinedEvent.h>

#include <Structs/ActionEvent.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/RequestOwnershipTransfer.h>
#include <Messages/NotifyOwnershipTransfer.h>
#include <Messages/RequestOwnershipClaim.h>
#include <Messages/MountRequest.h>
#include <Messages/NotifyMount.h>
#include <Messages/NewPackageRequest.h>
#include <Messages/NotifyNewPackage.h>
#include <Messages/RequestRespawn.h>
#include <Messages/NotifyRespawn.h>
#include <Messages/SyncExperienceRequest.h>
#include <Messages/NotifySyncExperience.h>
#include <Messages/DialogueRequest.h>
#include <Messages/NotifyDialogue.h>
#include <Messages/SubtitleRequest.h>
#include <Messages/NotifySubtitle.h>
#include <Messages/NotifyActorTeleport.h>

#include <World.h>
#include <Games/TES.h>

CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_referenceAddedConnection = m_dispatcher.sink<ActorAddedEvent>().connect<&CharacterService::OnActorAdded>(this);
    m_referenceRemovedConnection = m_dispatcher.sink<ActorRemovedEvent>().connect<&CharacterService::OnActorRemoved>(this);

    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&CharacterService::OnUpdate>(this);
    m_actionConnection = m_dispatcher.sink<ActionEvent>().connect<&CharacterService::OnActionEvent>(this);

    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&CharacterService::OnConnected>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&CharacterService::OnDisconnected>(this);

    m_assignCharacterConnection = m_dispatcher.sink<AssignCharacterResponse>().connect<&CharacterService::OnAssignCharacter>(this);
    m_characterSpawnConnection = m_dispatcher.sink<CharacterSpawnRequest>().connect<&CharacterService::OnCharacterSpawn>(this);
    m_referenceMovementSnapshotConnection = m_dispatcher.sink<ServerReferencesMoveRequest>().connect<&CharacterService::OnReferencesMoveRequest>(this);
    m_factionsConnection = m_dispatcher.sink<NotifyFactionsChanges>().connect<&CharacterService::OnFactionsChanges>(this);
    m_ownershipTransferConnection = m_dispatcher.sink<NotifyOwnershipTransfer>().connect<&CharacterService::OnOwnershipTransfer>(this);
    m_removeCharacterConnection = m_dispatcher.sink<NotifyRemoveCharacter>().connect<&CharacterService::OnRemoveCharacter>(this);

    m_mountConnection = m_dispatcher.sink<MountEvent>().connect<&CharacterService::OnMountEvent>(this);
    m_notifyMountConnection = m_dispatcher.sink<NotifyMount>().connect<&CharacterService::OnNotifyMount>(this);

    m_initPackageConnection = m_dispatcher.sink<InitPackageEvent>().connect<&CharacterService::OnInitPackageEvent>(this);
    m_newPackageConnection = m_dispatcher.sink<NotifyNewPackage>().connect<&CharacterService::OnNotifyNewPackage>(this);

    m_notifyRespawnConnection = m_dispatcher.sink<NotifyRespawn>().connect<&CharacterService::OnNotifyRespawn>(this);
    m_beastFormChangeConnection = m_dispatcher.sink<BeastFormChangeEvent>().connect<&CharacterService::OnBeastFormChange>(this);

    m_addExperienceEventConnection = m_dispatcher.sink<AddExperienceEvent>().connect<&CharacterService::OnAddExperienceEvent>(this);
    m_syncExperienceConnection = m_dispatcher.sink<NotifySyncExperience>().connect<&CharacterService::OnNotifySyncExperience>(this);

    m_dialogueEventConnection = m_dispatcher.sink<DialogueEvent>().connect<&CharacterService::OnDialogueEvent>(this);
    m_dialogueSyncConnection = m_dispatcher.sink<NotifyDialogue>().connect<&CharacterService::OnNotifyDialogue>(this);

    m_subtitleEventConnection = m_dispatcher.sink<SubtitleEvent>().connect<&CharacterService::OnSubtitleEvent>(this);
    m_subtitleSyncConnection = m_dispatcher.sink<NotifySubtitle>().connect<&CharacterService::OnNotifySubtitle>(this);

    m_actorTeleportConnection = m_dispatcher.sink<NotifyActorTeleport>().connect<&CharacterService::OnNotifyActorTeleport>(this);

    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&CharacterService::OnPartyJoinedEvent>(this);
}

void CharacterService::DeleteRemoteEntityComponents(entt::entity aEntity) const noexcept
{
    m_world.remove<FaceGenComponent, InterpolationComponent, RemoteAnimationComponent, RemoteComponent, CacheComponent, WaitingFor3D, PlayerComponent>(aEntity);
}

void CharacterService::DeclineOwnership(const uint32_t aServerId, const uint32_t aOwnershipEpoch) const noexcept
{
    RequestOwnershipTransfer request{};
    request.ServerId = aServerId;
    request.OwnershipEpoch = aOwnershipEpoch;
    request.Reason = OwnershipReleaseReason::DeclineGrant;
    m_transport.Send(request);
}

void CharacterService::ReconcileActorData(
    const entt::entity aEntity, Actor* apActor, const uint32_t aOwnershipEpoch, const ActorData& acActorData, const bool aApplyInventory, const bool aIsLocalOwner) noexcept
{
    if (auto* pWaitingFor3D = m_world.try_get<WaitingFor3D>(aEntity))
    {
        pWaitingFor3D->SpawnRequest.InitialActorValues = acActorData.InitialActorValues;
        pWaitingFor3D->SpawnRequest.InventoryContent = acActorData.InitialInventory;
        pWaitingFor3D->SpawnRequest.IsDead = acActorData.IsDead;
        pWaitingFor3D->SpawnRequest.IsWeaponDrawn = acActorData.IsWeaponDrawn;
        pWaitingFor3D->SpawnRequest.OwnershipEpoch = aOwnershipEpoch;
    }

    if (!apActor)
        return;

    apActor->SetActorValues(acActorData.InitialActorValues);

    if (aApplyInventory)
    {
        const Inventory currentInventory = apActor->GetActorInventory();
        if (currentInventory.Entries != acActorData.InitialInventory.Entries || currentInventory.CurrentMagicEquipment != acActorData.InitialInventory.CurrentMagicEquipment)
            apActor->SetActorInventory(acActorData.InitialInventory);
    }

    if (apActor->IsDead() != acActorData.IsDead)
        acActorData.IsDead ? apActor->Kill() : apActor->Respawn();

    if (aIsLocalOwner)
    {
        // A remote draw correction may still be queued when an ownership grant arrives.
        m_weaponDrawUpdates.erase(apActor->formID);

        if (apActor->actorState.IsWeaponDrawn() != acActorData.IsWeaponDrawn)
            apActor->SetWeaponDrawnEx(acActorData.IsWeaponDrawn);
    }
    else
        m_weaponDrawUpdates[apActor->formID] = {acActorData.IsWeaponDrawn};
}

bool CharacterService::RequestOwnership(const uint32_t aFormId, const uint32_t aServerId, const entt::entity aEntity) const noexcept
{
    Actor* pActor = Cast<Actor>(TESForm::GetById(aFormId));
    if (!pActor)
    {
        spdlog::warn("Cannot request ownership of actor {:X} because its form {:X} is unavailable", aServerId, aFormId);
        return false;
    }

    ActorExtension* pExtension = pActor->GetExtension();
    if (pExtension->IsRemotePlayer())
    {
        spdlog::warn("Cannot request ownership of remote player actor {:X}", aServerId);
        return false;
    }

    if (pActor->IsPlayerSummon())
    {
        spdlog::warn("Cannot request ownership of remote player summon {:X}", aServerId);
        return false;
    }

    const auto* pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity);
    if (!pRemoteComponent || pRemoteComponent->Id != aServerId || pRemoteComponent->OwnershipEpoch == 0)
        return false;

    RequestOwnershipClaim request;
    request.ServerId = aServerId;
    request.ExpectedOwnershipEpoch = pRemoteComponent->OwnershipEpoch;

    if (!m_transport.Send(request))
        return false;

    return true;
}

void CharacterService::DeleteTempActor(const uint32_t aFormId) noexcept
{
    Actor* pActor = Cast<Actor>(TESForm::GetById(aFormId));
    if (pActor && ((pActor->formID & 0xFF000000) == 0xFF000000))
    {
        pActor->Delete();
        spdlog::info("\tDeleted actor {:X}", aFormId);
    }
}

void CharacterService::OnActorAdded(const ActorAddedEvent& acEvent) noexcept
{
    Actor* pActor = Cast<Actor>(TESForm::GetById(acEvent.FormId));

    if (acEvent.FormId == 0x14)
    {
        pActor->GetExtension()->SetPlayer(true);
    }

    entt::entity entity;

    const auto view = m_world.view<RemoteComponent>();
    const auto it = std::find_if(
        std::begin(view), std::end(view),
        [&acEvent, view](entt::entity entity)
        {
            auto& remoteComponent = view.get<RemoteComponent>(entity);
            return remoteComponent.CachedRefId == acEvent.FormId;
        });

    if (it != std::end(view))
    {
        Actor* pActor = Cast<Actor>(TESForm::GetById(acEvent.FormId));
        pActor->GetExtension()->SetRemote(true);

        entity = *it;
    }
    else
        entity = m_world.create();

    m_world.emplace_or_replace<FormIdComponent>(entity, acEvent.FormId);
    m_world.emplace_or_replace<EarlyAnimationBufferComponent>(entity);

    ProcessNewEntity(entity);
}

void CharacterService::OnActorRemoved(const ActorRemovedEvent& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent>();
    const auto entityIt = std::find_if(view.begin(), view.end(), [view, formId = acEvent.FormId](auto aEntity) { return view.get<FormIdComponent>(aEntity).Id == formId; });

    if (entityIt == view.end())
    {
        spdlog::error("Actor to remove not found in form ids map {:X}", acEvent.FormId);
        return;
    }

    const auto cId = *entityIt;

    auto& formIdComponent = view.get<FormIdComponent>(cId);
    CancelServerAssignment(*entityIt, formIdComponent.Id);

    m_world.remove<EarlyAnimationBufferComponent>(cId);

    if (m_world.all_of<FormIdComponent>(cId))
        m_world.remove<FormIdComponent>(cId);

    if (m_world.orphan(cId))
        m_world.destroy(cId);

    spdlog::info("Actor removed, form id: {:X}", acEvent.FormId);
}

void CharacterService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunSpawnUpdates();
    RunLocalUpdates();
    RunFactionsUpdates();
    RunRemoteUpdates();
    RunExperienceUpdates();
    ApplyCachedWeaponDraws(acUpdateEvent);
    ProcessLeveledConforms();
}

void CharacterService::OnConnected(const ConnectedEvent& acConnectedEvent) const noexcept
{
    // Go through all the forms that were previously detected
    auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);
    Vector<entt::entity> entities(view.begin(), view.end());

    for (auto entity : entities)
    {
        auto& formIdComponent = m_world.get<FormIdComponent>(entity);
        // Delete all temporary actors on connect
        if (formIdComponent.Id > 0xFF000000)
        {
            Actor* pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
            if (pActor)
                pActor->Delete();

            continue;
        }

        ProcessNewEntity(entity);
    }
}

void CharacterService::OnDisconnected(const DisconnectedEvent& acDisconnectedEvent) const noexcept
{
    auto remoteView = m_world.view<FormIdComponent, RemoteComponent>();
    for (auto entity : remoteView)
    {
        auto& formIdComponent = remoteView.get<FormIdComponent>(entity);

        auto pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
        if (!pActor)
            continue;

        if (pActor->GetExtension()->IsRemotePlayer())
            pActor->Delete();
        else
            pActor->GetExtension()->SetRemote(false);
    }

    m_world.clear<WaitingForAssignmentComponent, LocalComponent, RemoteComponent>();

    m_pendingLeveledConforms.clear();
}

void CharacterService::OnAssignCharacter(const AssignCharacterResponse& acMessage) noexcept
{
    spdlog::info("Received for cookie {:X}, server id {:X}", acMessage.Cookie, acMessage.ServerId);

    auto view = m_world.view<WaitingForAssignmentComponent>();
    const auto itor = std::find_if(std::begin(view), std::end(view), [view, cookie = acMessage.Cookie](auto entity) { return view.get<WaitingForAssignmentComponent>(entity).Cookie == cookie; });

    if (itor == std::end(view))
    {
        spdlog::warn("Never found requested cookie: {}", acMessage.Cookie);
        return;
    }

    const auto cEntity = *itor;
    const bool isCancelled = view.get<WaitingForAssignmentComponent>(cEntity).Cancelled;

    m_world.remove<WaitingForAssignmentComponent>(cEntity);
#if (!IS_MASTER)
    m_world.remove<ReplayedActionsDebugComponent>(cEntity);
#endif

    if (isCancelled)
    {
        if (acMessage.Owner)
            DeclineOwnership(acMessage.ServerId, acMessage.OwnershipEpoch);

        if (m_world.valid(cEntity))
            m_world.destroy(cEntity);

        return;
    }

    if (acMessage.OwnershipEpoch == 0)
    {
        spdlog::warn("Ignored assignment for actor {:X} because the server returned an invalid ownership epoch", acMessage.ServerId);
        return;
    }

    const auto formIdComponent = m_world.try_get<FormIdComponent>(cEntity);
    if (!formIdComponent)
    {
        if (acMessage.Owner)
            DeclineOwnership(acMessage.ServerId, acMessage.OwnershipEpoch);

        if (m_world.valid(cEntity))
            m_world.destroy(cEntity);

        spdlog::warn("Discarded assignment for actor {:X} because the local entity no longer has a form", acMessage.ServerId);
        return;
    }

    Actor* pActor = Cast<Actor>(TESForm::GetById(formIdComponent->Id));
    if (!pActor)
    {
        if (acMessage.Owner)
            DeclineOwnership(acMessage.ServerId, acMessage.OwnershipEpoch);

        spdlog::warn("Discarded assignment for actor {:X} because form {:X} is unavailable", acMessage.ServerId, formIdComponent->Id);
        m_world.destroy(cEntity);
        return;
    }

    // TODO: how could this possibly trigger?
    // it's kinda interfering with my WaitingFor3D code
    if (acMessage.PlayerId != 0)
        m_world.emplace_or_replace<PlayerComponent>(cEntity, acMessage.PlayerId);

    ActorData actorData{};
    actorData.InitialActorValues = acMessage.AllActorValues;
    actorData.InitialInventory = acMessage.CurrentInventory;
    actorData.IsDead = acMessage.IsDead;
    actorData.IsWeaponDrawn = acMessage.IsWeaponDrawn;

    if (acMessage.Owner)
    {
        spdlog::info("Received local actor, form id: {:X}", pActor->formID);

        pActor->GetExtension()->SetRemote(true);
        ReconcileActorData(cEntity, pActor, acMessage.OwnershipEpoch, actorData, true, true);

        auto& localAnimationComponent = m_world.emplace_or_replace<LocalAnimationComponent>(cEntity);

        if (auto* pEarlyAnimComponent = m_world.try_get<EarlyAnimationBufferComponent>(cEntity))
        {
            for (const auto& action : pEarlyAnimComponent->Actions)
            {
                localAnimationComponent.Append(action);
            }
        }
        m_world.remove<EarlyAnimationBufferComponent>(cEntity);

        auto& localComponent = m_world.emplace_or_replace<LocalComponent>(cEntity, acMessage.ServerId, acMessage.OwnershipEpoch);
        localComponent.IsDead = acMessage.IsDead;
        localComponent.IsWeaponDrawn = acMessage.IsWeaponDrawn;
        pActor->GetExtension()->SetRemote(false);
    }
    else
    {
        spdlog::info("Received remote actor, form id: {:X}, isweapondrawn: {}", pActor->formID, acMessage.IsWeaponDrawn);

        m_world.emplace_or_replace<RemoteComponent>(cEntity, acMessage.ServerId, formIdComponent->Id, acMessage.OwnershipEpoch);

        pActor->GetExtension()->SetRemote(true);

        m_world.remove<EarlyAnimationBufferComponent>(cEntity);
        InterpolationSystem::Setup(m_world, cEntity);
        AnimationSystem::Setup(m_world, cEntity);
        AnimationSystem::AddActionsForReplay(m_world.get<RemoteAnimationComponent>(cEntity), acMessage.ActionsToReplay);

#if (!IS_MASTER)
        m_world.emplace_or_replace<ReplayedActionsDebugComponent>(cEntity, acMessage.ActionsToReplay);
#endif

        ReconcileActorData(cEntity, pActor, acMessage.OwnershipEpoch, actorData, true, false);

        MoveActor(pActor, acMessage.WorldSpaceId, acMessage.CellId, acMessage.Position);

        // The owner's leveled pick rides the assignment response for actors we discovered ourselves
        ApplyLeveledNpcPick(pActor, acMessage.LeveledNpcPickId);
    }
}

void CharacterService::OnCharacterSpawn(const CharacterSpawnRequest& acMessage) const noexcept
{
    if (acMessage.OwnershipEpoch == 0)
    {
        spdlog::warn("Ignored spawn for actor {:X} because the ownership epoch is invalid", acMessage.ServerId);
        return;
    }

    auto remoteView = m_world.view<RemoteComponent>();
    const auto remoteItor = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ServerId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteItor != std::end(remoteView))
    {
        spdlog::warn("Character with remote id {:X} is already spawned.", acMessage.ServerId);
        return;
    }

    Actor* pActor = nullptr;

    std::optional<entt::entity> entity;

    // Custom forms
    if (acMessage.FormId == GameId{})
    {
        TESNPC* pNpc = nullptr;

        entity = m_world.create();

        if (acMessage.BaseId != GameId{})
        {
            // Prefer the owner's resolved leveled pick over the lossy template base
            GameId baseId = acMessage.BaseId;
            uint32_t npcId = World::Get().GetModSystem().GetGameId(baseId);
            if (acMessage.LeveledNpcPickId != GameId{})
            {
                if (const uint32_t cPickNpcId = World::Get().GetModSystem().GetGameId(acMessage.LeveledNpcPickId))
                {
                    baseId = acMessage.LeveledNpcPickId;
                    npcId = cPickNpcId;
                }
            }

            if (npcId == 0)
            {
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod, base: {:X}:{:X}, form: {:X}:{:X}", baseId.BaseId, baseId.ModId, acMessage.FormId.BaseId, acMessage.FormId.ModId);
                return;
            }

            pNpc = Cast<TESNPC>(TESForm::GetById(npcId));
            pNpc->Deserialize(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
        }
        else
        {
            // Players and npcs with temporary ref ids and base ids (usually random events)
            pNpc = TESNPC::Create(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
            FaceGenSystem::Setup(m_world, *entity, acMessage.FaceTints);
        }

        pActor = Actor::Create(pNpc);
    }
    else
    {
        const uint32_t cActorId = World::Get().GetModSystem().GetGameId(acMessage.FormId);

        auto waitingView = m_world.view<FormIdComponent, WaitingForAssignmentComponent>();
        const auto waitingItor = std::find_if(std::begin(waitingView), std::end(waitingView), [waitingView, cActorId](auto entity) { return waitingView.get<FormIdComponent>(entity).Id == cActorId; });

        if (waitingItor != std::end(waitingView))
        {
            spdlog::info("Character with form id {:X} already has a spawn request in progress.", cActorId);
            return;
        }

        auto* const pForm = TESForm::GetById(cActorId);
        pActor = Cast<Actor>(pForm);

        if (!pActor)
        {
            spdlog::error("Failed to retrieve Actor {:X}, it will not be spawned, possibly missing mod", cActorId);
            spdlog::error("\tForm : {:X}", pForm ? pForm->formID : 0);
            return;
        }

        const auto view = m_world.view<FormIdComponent>();
        const auto itor = std::find_if(std::begin(view), std::end(view), [cActorId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == cActorId; });

        if (itor != std::end(view))
            entity = *itor;
        else
            entity = m_world.create();
    }

    if (!pActor)
    {
        spdlog::error("Actor object {:X} could not be created.", acMessage.ServerId);
        return;
    }

    spdlog::info("CharacterSpawnRequest, server id: {:X}, form id: {:X}", acMessage.ServerId, pActor->formID);

    if (pActor->IsDisabled())
    {
        spdlog::warn("Disabled actor is being re-enabled: {:X}", pActor->formID);
        pActor->EnableImpl();
    }

    pActor->GetExtension()->SetRemote(true);

    pActor->rotation.x = acMessage.Rotation.x;
    pActor->rotation.z = acMessage.Rotation.y;
    pActor->MoveTo(PlayerCharacter::Get()->parentCell, acMessage.Position);
    pActor->SetActorValues(acMessage.InitialActorValues);

    pActor->GetExtension()->SetPlayer(acMessage.IsPlayer);
    if (acMessage.IsPlayer)
    {
        pActor->SetIgnoreFriendlyHit(true);
        pActor->SetPlayerRespawnMode();
        m_world.emplace_or_replace<PlayerComponent>(*entity, acMessage.PlayerId);
    }

    if (pActor->IsDead() != acMessage.IsDead)
        acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

    spdlog::info("Spawn Request Is summon {}", acMessage.IsPlayerSummon);

    if (acMessage.IsPlayerSummon)
    {
        // Prevents remote summons agroing other players.
        pActor->SetCommandingActor(PlayerCharacter::Get()->GetHandle());
    }

    // Static references arrive with their own locally rolled leveled pick; conform to the owner's.
    if (acMessage.FormId != GameId{})
        ApplyLeveledNpcPick(pActor, acMessage.LeveledNpcPickId);

    m_world.emplace_or_replace<RemoteComponent>(*entity, acMessage.ServerId, pActor->formID, acMessage.OwnershipEpoch);

    auto& interpolationComponent = InterpolationSystem::Setup(m_world, *entity);
    interpolationComponent.Position = acMessage.Position;

    AnimationSystem::Setup(m_world, *entity);

    m_world.emplace_or_replace<WaitingFor3D>(*entity, acMessage);

    auto& remoteAnimationComponent = m_world.get<RemoteAnimationComponent>(*entity);

    AnimationSystem::AddActionsForReplay(remoteAnimationComponent, acMessage.ActionsToReplay);

#if (!IS_MASTER)
    m_world.emplace_or_replace<ReplayedActionsDebugComponent>(*entity, acMessage.ActionsToReplay);
#endif
}

void CharacterService::OnReferencesMoveRequest(const ServerReferencesMoveRequest& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>();

    for (const auto& [serverId, update] : acMessage.Updates)
    {
        auto itor = std::find_if(std::begin(view), std::end(view), [serverId = serverId, view](entt::entity entity) { return view.get<RemoteComponent>(entity).Id == serverId; });

        if (itor == std::end(view))
            continue;

        auto& interpolationComponent = view.get<InterpolationComponent>(*itor);
        auto& animationComponent = view.get<RemoteAnimationComponent>(*itor);
        const auto& movement = update.UpdatedMovement;

        InterpolationComponent::TimePoint point;
        point.Tick = acMessage.Tick;
        point.Position = movement.Position;
        point.Rotation = {movement.Rotation.x, 0.f, movement.Rotation.y};
        point.Variables = movement.Variables;
        point.Direction = movement.Direction;

        InterpolationSystem::AddPoint(interpolationComponent, point);

        for (const auto& action : update.ActionEvents)
        {
            animationComponent.TimePoints.push_back(action);
        }
    }
}

void CharacterService::OnActionEvent(const ActionEvent& acActionEvent) const noexcept
{
    auto view = m_world.view<LocalAnimationComponent, FormIdComponent>();
    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acActionEvent.ActorId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (itor != std::end(view))
    {
        auto& localComponent = view.get<LocalAnimationComponent>(*itor);

        localComponent.Append(acActionEvent);
    }
    else if (m_transport.IsOnline())
    {
        // A `LocalAnimationComponent` is not attached yet, but the actor already exists and is running animations

        auto view = m_world.view<FormIdComponent, EarlyAnimationBufferComponent>();
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = acActionEvent.ActorId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == id; });

        if (itor != std::end(view))
        {
            view.get<EarlyAnimationBufferComponent>(*itor).Actions.push_back(acActionEvent);
        }
    }
}

void CharacterService::OnFactionsChanges(const NotifyFactionsChanges& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent, CacheComponent>();

    for (const auto& [id, factions] : acEvent.Changes)
    {
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = id, view](entt::entity entity) { return view.get<RemoteComponent>(entity).Id == id; });

        if (itor != std::end(view))
        {
            auto& formIdComponent = view.get<FormIdComponent>(*itor);

            auto* const pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
            if (!pActor)
                return;

            auto& cacheComponent = view.get<CacheComponent>(*itor);
            cacheComponent.FactionsContent = factions;

            pActor->SetFactions(cacheComponent.FactionsContent);
        }
    }
}

void CharacterService::OnOwnershipTransfer(const NotifyOwnershipTransfer& acMessage) noexcept
{
    if (acMessage.OwnershipEpoch == 0)
    {
        spdlog::warn("Ignored ownership update for actor {:X} because the epoch is invalid", acMessage.ServerId);
        return;
    }

    auto entity = Utils::FindEntityByServerId(acMessage.ServerId);
    if (entity && !m_world.any_of<LocalComponent, RemoteComponent>(*entity))
        entity.reset();

    uint32_t currentEpoch = 0;
    if (entity)
    {
        if (const auto* pLocalComponent = m_world.try_get<LocalComponent>(*entity))
            currentEpoch = pLocalComponent->OwnershipEpoch;
        else if (const auto* pRemoteComponent = m_world.try_get<RemoteComponent>(*entity))
            currentEpoch = pRemoteComponent->OwnershipEpoch;
    }

    if (currentEpoch != 0 && acMessage.OwnershipEpoch <= currentEpoch)
    {
        spdlog::debug("Ignored stale ownership update for actor {:X} at epoch {}; current epoch is {}", acMessage.ServerId, acMessage.OwnershipEpoch, currentEpoch);
        return;
    }

    const bool isLocalOwner = acMessage.OwnerPlayerId == m_transport.GetLocalPlayerId();
    if (!entity)
    {
        // A transfer does not contain enough form data to recreate an unknown actor. Decline so the server can try another loaded client.
        if (isLocalOwner)
            DeclineOwnership(acMessage.ServerId, acMessage.OwnershipEpoch);
        else
            spdlog::debug("Ignored ownership update for unknown actor {:X} at epoch {}", acMessage.ServerId, acMessage.OwnershipEpoch);
        return;
    }

    const entt::entity cEntity = *entity;
    const auto* pFormIdComponent = m_world.try_get<FormIdComponent>(cEntity);
    Actor* pActor = pFormIdComponent ? Cast<Actor>(TESForm::GetById(pFormIdComponent->Id)) : nullptr;

    // Preserve the accepted epoch's pick for actors that still need to be created.
    if (auto* pWaitingFor3D = m_world.try_get<WaitingFor3D>(cEntity))
        pWaitingFor3D->SpawnRequest.LeveledNpcPickId = acMessage.LeveledNpcPickId;

    if (isLocalOwner)
    {
        if (!pFormIdComponent || !pActor || !pActor->GetNiNode())
        {
            uint32_t cachedRefId = pFormIdComponent ? pFormIdComponent->Id : 0;
            if (const auto* pRemoteComponent = m_world.try_get<RemoteComponent>(cEntity))
                cachedRefId = pRemoteComponent->CachedRefId;

            if (pActor)
                pActor->GetExtension()->SetRemote(true);

            m_world.remove<LocalAnimationComponent, LocalComponent>(cEntity);
            if (m_world.all_of<RemoteComponent>(cEntity))
                m_world.get<RemoteComponent>(cEntity).OwnershipEpoch = acMessage.OwnershipEpoch;
            else if (cachedRefId != 0)
                m_world.emplace<RemoteComponent>(cEntity, acMessage.ServerId, cachedRefId, acMessage.OwnershipEpoch);

            spdlog::warn("Declined ownership of actor {:X} at epoch {} because the actor is not ready", acMessage.ServerId, acMessage.OwnershipEpoch);
            DeclineOwnership(acMessage.ServerId, acMessage.OwnershipEpoch);
            return;
        }

        // Reconcile while hooks still treat the actor as remote/non-authoritative.
        pActor->GetExtension()->SetRemote(true);
        m_world.remove<LocalAnimationComponent, LocalComponent>(cEntity);
        m_world.emplace_or_replace<RemoteComponent>(cEntity, acMessage.ServerId, pFormIdComponent->Id, acMessage.OwnershipEpoch);

        ReconcileActorData(cEntity, pActor, acMessage.OwnershipEpoch, acMessage.CurrentActorData, true, true);
        ApplyLeveledNpcPick(pActor, acMessage.LeveledNpcPickId);

        DeleteRemoteEntityComponents(cEntity);
        CacheSystem::Setup(m_world, cEntity, pActor);
        m_world.emplace_or_replace<LocalAnimationComponent>(cEntity);
        auto& localComponent = m_world.emplace_or_replace<LocalComponent>(cEntity, acMessage.ServerId, acMessage.OwnershipEpoch);
        localComponent.IsDead = acMessage.CurrentActorData.IsDead;
        localComponent.IsWeaponDrawn = acMessage.CurrentActorData.IsWeaponDrawn;

        // LocalComponent is installed only after canonical reconciliation is complete.
        pActor->GetExtension()->SetRemote(false);
        spdlog::info("Gained ownership of actor {:X} at epoch {}", acMessage.ServerId, acMessage.OwnershipEpoch);
        return;
    }

    if (pActor)
        pActor->GetExtension()->SetRemote(true);

    m_world.remove<LocalAnimationComponent, LocalComponent>(cEntity);

    if (pFormIdComponent)
    {
        m_world.emplace_or_replace<RemoteComponent>(cEntity, acMessage.ServerId, pFormIdComponent->Id, acMessage.OwnershipEpoch);

        if (!m_world.all_of<InterpolationComponent>(cEntity))
            InterpolationSystem::Setup(m_world, cEntity);
        if (!m_world.all_of<RemoteAnimationComponent>(cEntity))
            AnimationSystem::Setup(m_world, cEntity);
    }
    else if (auto* pRemoteComponent = m_world.try_get<RemoteComponent>(cEntity))
    {
        pRemoteComponent->OwnershipEpoch = acMessage.OwnershipEpoch;
    }

    ReconcileActorData(cEntity, pActor, acMessage.OwnershipEpoch, acMessage.CurrentActorData, pActor && pActor->GetNiNode(), false);
    if (pActor)
        ApplyLeveledNpcPick(pActor, acMessage.LeveledNpcPickId);

    spdlog::info("Actor {:X} is now owned by player {:X} at epoch {}", acMessage.ServerId, acMessage.OwnerPlayerId, acMessage.OwnershipEpoch);
}

void CharacterService::OnRemoveCharacter(const NotifyRemoveCharacter& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acMessage.ServerId, view](entt::entity entity) { return view.get<RemoteComponent>(entity).Id == id; });

    if (itor != std::end(view))
    {
        if (auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*itor))
        {
            Actor* pActor = Cast<Actor>(TESForm::GetById(pFormIdComponent->Id));
            if (pActor && pActor->IsTemporary())
                CharacterService::DeleteTempActor(pFormIdComponent->Id);
            else if (pActor)
                pActor->GetExtension()->SetRemote(false);
        }

        DeleteRemoteEntityComponents(*itor);
    }
}

void CharacterService::OnNotifyRespawn(const NotifyRespawn& acMessage) const noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();
    const auto entityIt = std::find_if(view.begin(), view.end(), [view, id = acMessage.ActorId](auto aEntity) { return view.get<RemoteComponent>(aEntity).Id == id; });

    if (entityIt == view.end())
    {
        spdlog::error("Actor to respawn not found in: {:X}", acMessage.ActorId);
        return;
    }

    const auto cId = *entityIt;

    auto& formIdComponent = view.get<FormIdComponent>(cId);
    CancelServerAssignment(*entityIt, formIdComponent.Id);

    m_world.remove<EarlyAnimationBufferComponent>(cId);

    if (m_world.all_of<FormIdComponent>(cId))
        m_world.remove<FormIdComponent>(cId);

    if (m_world.orphan(cId))
        m_world.destroy(cId);

    RequestRespawn request;
    request.ActorId = acMessage.ActorId;

    m_transport.Send(request);
}

void CharacterService::OnBeastFormChange(const BeastFormChangeEvent& acEvent) const noexcept
{
    auto view = m_world.view<FormIdComponent>();

    const auto it = std::find_if(view.begin(), view.end(), [view](auto entity) { return view.get<FormIdComponent>(entity).Id == 0x14; });

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*it);
    if (!serverIdRes.has_value())
    {
        spdlog::error("{}: failed to find server id", __FUNCTION__);
        return;
    }

    uint32_t serverId = serverIdRes.value();

    RequestRespawn request;
    request.ActorId = serverId;

    Actor* pActor = Utils::GetByServerId<Actor>(serverId);
    if (!pActor)
    {
        spdlog::warn(__FUNCTION__ ": could not find actor for server id {:X}", serverId);
        return;
    }

    TESNPC* pNpc = Cast<TESNPC>(pActor->baseForm);
    if (!pNpc)
    {
        spdlog::warn(__FUNCTION__ ": could not find actor baseform for server id {:X}", serverId);
        return;
    }

    pNpc->Serialize(&request.AppearanceBuffer);
    request.ChangeFlags = pNpc->GetChangeFlags();

    m_transport.Send(request);
}

void CharacterService::OnMountEvent(const MountEvent& acEvent) const noexcept
{
    auto view = m_world.view<FormIdComponent>();

    const auto riderIt = std::find_if(std::begin(view), std::end(view), [id = acEvent.RiderID, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (riderIt == std::end(view))
    {
        spdlog::warn("Rider not found, form id: {:X}", acEvent.RiderID);
        return;
    }

    const entt::entity cRiderEntity = *riderIt;
    const auto* pRiderLocalComponent = m_world.try_get<LocalComponent>(cRiderEntity);
    if (!pRiderLocalComponent)
        return;

    const auto mountIt = std::find_if(std::begin(view), std::end(view), [id = acEvent.MountID, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (mountIt == std::end(view))
    {
        spdlog::warn("Mount not found, form id: {:X}", acEvent.MountID);
        return;
    }

    const entt::entity cMountEntity = *mountIt;

    uint32_t mountServerId = 0;
    uint32_t mountOwnershipEpoch = 0;
    if (const auto* pMountLocalComponent = m_world.try_get<LocalComponent>(cMountEntity))
    {
        mountServerId = pMountLocalComponent->Id;
        mountOwnershipEpoch = pMountLocalComponent->OwnershipEpoch;
    }
    else if (const auto* pMountRemoteComponent = m_world.try_get<RemoteComponent>(cMountEntity))
    {
        mountServerId = pMountRemoteComponent->Id;
        mountOwnershipEpoch = pMountRemoteComponent->OwnershipEpoch;
    }
    else
        return;

    MountRequest request{};
    request.RiderId = pRiderLocalComponent->Id;
    request.RiderOwnershipEpoch = pRiderLocalComponent->OwnershipEpoch;
    request.MountId = mountServerId;
    request.MountOwnershipEpoch = mountOwnershipEpoch;

    m_transport.Send(request);
}

void CharacterService::OnNotifyMount(const NotifyMount& acMessage) const noexcept
{
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();

    const auto riderIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.RiderId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (riderIt == std::end(remoteView))
    {
        spdlog::warn("Rider with remote id {:X} not found.", acMessage.RiderId);
        return;
    }

    auto& riderFormIdComponent = remoteView.get<FormIdComponent>(*riderIt);
    TESForm* pRiderForm = TESForm::GetById(riderFormIdComponent.Id);
    Actor* pRider = Cast<Actor>(pRiderForm);
    if (!pRider)
        return;

    const auto mountIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.MountId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });
    if (mountIt == std::end(remoteView))
    {
        spdlog::warn("Cannot apply mount update because mount {:X} is unavailable", acMessage.MountId);
        return;
    }

    const auto& mountFormIdComponent = remoteView.get<FormIdComponent>(*mountIt);
    Actor* pMount = Cast<Actor>(TESForm::GetById(mountFormIdComponent.Id));
    if (!pMount)
        return;

    pRider->InitiateMountPackage(pMount);
}

void CharacterService::OnInitPackageEvent(const InitPackageEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();

    const auto actorIt = std::find_if(std::begin(view), std::end(view), [id = acEvent.ActorId, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (actorIt == std::end(view))
        return;

    const entt::entity cActorEntity = *actorIt;

    std::optional<uint32_t> actorServerIdRes = Utils::GetServerId(cActorEntity);
    if (!actorServerIdRes.has_value())
    {
        spdlog::error("{}: failed to find server id", __FUNCTION__);
        return;
    }

    NewPackageRequest request;
    request.ActorId = actorServerIdRes.value();
    if (!m_world.GetModSystem().GetServerModId(acEvent.PackageId, request.PackageId.ModId, request.PackageId.BaseId))
        return;

    m_transport.Send(request);
}

void CharacterService::OnNotifyNewPackage(const NotifyNewPackage& acMessage) const noexcept
{
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ActorId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Actor for package with remote id {:X} not found.", acMessage.ActorId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

    const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    Actor* pActor = Cast<Actor>(pForm);

    const uint32_t cPackageFormId = World::Get().GetModSystem().GetGameId(acMessage.PackageId);
    const TESForm* pPackageForm = TESForm::GetById(cPackageFormId);
    if (!pPackageForm)
    {
        spdlog::warn("Actor package not found, base id: {:X}, mod id: {:X}", acMessage.PackageId.BaseId, acMessage.PackageId.ModId);
        return;
    }

    TESPackage* pPackage = Cast<TESPackage>(pPackageForm);

    pActor->SetPackage(pPackage);
}

void CharacterService::OnAddExperienceEvent(const AddExperienceEvent& acEvent) noexcept
{
    m_cachedExperience += acEvent.Experience;
}

void CharacterService::OnNotifySyncExperience(const NotifySyncExperience& acMessage) noexcept
{
    PlayerCharacter* pPlayer = PlayerCharacter::Get();

    if (PlayerCharacter::LastUsedCombatSkill == -1)
        return;

    pPlayer->AddSkillExperience(PlayerCharacter::LastUsedCombatSkill, acMessage.Experience);
}

void CharacterService::OnDialogueEvent(const DialogueEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);
    auto entityIt = std::find_if(view.begin(), view.end(), [view, formId = acEvent.ActorID](auto entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (entityIt == view.end())
        return;

    auto serverIdRes = Utils::GetServerId(*entityIt);
    if (!serverIdRes)
    {
        spdlog::error("{}: server id not found for form id {:X}", __FUNCTION__, acEvent.ActorID);
        return;
    }

    DialogueRequest request{};
    request.ServerId = serverIdRes.value();
    request.SoundFilename = acEvent.VoiceFile;

    m_transport.Send(request);
}

void CharacterService::OnNotifyDialogue(const NotifyDialogue& acMessage) noexcept
{
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ServerId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Actor for dialogue with remote id {:X} not found.", acMessage.ServerId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);
    const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    Actor* pActor = Cast<Actor>(pForm);

    if (!pActor)
        return;

    pActor->StopCurrentDialogue(true);
    pActor->SpeakSound(acMessage.SoundFilename.c_str());
}

void CharacterService::OnSubtitleEvent(const SubtitleEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);
    auto entityIt = std::find_if(view.begin(), view.end(), [view, formId = acEvent.SpeakerID](auto entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (entityIt == view.end())
        return;

    auto serverIdRes = Utils::GetServerId(*entityIt);
    if (!serverIdRes)
    {
        spdlog::error("{}: server id not found for form id {:X}", __FUNCTION__, acEvent.SpeakerID);
        return;
    }

    SubtitleRequest request{};
    request.ServerId = serverIdRes.value();
    request.Text = acEvent.Text;
    request.TopicFormId = acEvent.TopicFormID;

    m_transport.Send(request);
}

void CharacterService::OnNotifySubtitle(const NotifySubtitle& acMessage) noexcept
{
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ServerId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Actor for dialogue with remote id {:X} not found.", acMessage.ServerId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);
    const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    Actor* pActor = Cast<Actor>(pForm);

    if (!pActor)
        return;

    // This is only for fallout 4
    TESTopicInfo* pInfo = nullptr;
    pInfo = Cast<TESTopicInfo>(TESForm::GetById(acMessage.TopicFormId));

    SubtitleManager::Get()->ShowSubtitle(pActor, acMessage.Text.c_str(), pInfo);
}

void CharacterService::OnNotifyActorTeleport(const NotifyActorTeleport& acMessage) noexcept
{
    auto& modSystem = m_world.GetModSystem();

    const uint32_t cActorId = World::Get().GetModSystem().GetGameId(acMessage.FormId);
    Actor* pActor = Cast<Actor>(TESForm::GetById(cActorId));
    if (!pActor)
    {
        spdlog::error(__FUNCTION__ ": failed to retrieve actor to teleport.");
        return;
    }

    MoveActor(pActor, acMessage.WorldSpaceId, acMessage.CellId, acMessage.Position);

    spdlog::info("Successfully teleported actor, form id: {:X}, world space: {:X}, cell: {:X}, position: ({}, {}, {})", pActor->formID, acMessage.WorldSpaceId.BaseId, acMessage.CellId.BaseId, acMessage.Position.x, acMessage.Position.y, acMessage.Position.z);
}

void CharacterService::OnPartyJoinedEvent(const PartyJoinedEvent& acEvent) noexcept
{
    // Takes ownership of all actors
    if (acEvent.IsLeader)
    {
        auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);
        Vector<entt::entity> entities(view.begin(), view.end());

        for (auto entity : entities)
            ProcessNewEntity(entity);
    }
}

void CharacterService::MoveActor(const Actor* apActor, const GameId& acWorldSpaceId, const GameId& acCellId, const Vector3_NetQuantize& acPosition) const noexcept
{
    TESObjectCELL* pCell = nullptr;
    if (!acWorldSpaceId)
    {
        const uint32_t cCellId = m_world.GetModSystem().GetGameId(acCellId);
        pCell = Cast<TESObjectCELL>(TESForm::GetById(cCellId));
    }
    // In case of lazy-loading of exterior cells
    else
    {
        const uint32_t cWorldSpaceId = m_world.GetModSystem().GetGameId(acWorldSpaceId);
        TESWorldSpace* const pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(cWorldSpaceId));
        if (pWorldSpace)
        {
            GridCellCoords coordinates = GridCellCoords::CalculateGridCellCoords(acPosition);
            pCell = pWorldSpace->LoadCell(coordinates.X, coordinates.Y);
        }
    }

    if (!pCell)
    {
        spdlog::error(__FUNCTION__ ": failed to fetch cell to teleport, actor: {:X}, worldspace: {:X}, cell: {:X}, position: {}, {}, {}", apActor->formID, acWorldSpaceId.BaseId, acCellId.BaseId, acPosition.x, acPosition.y, acPosition.z);
        return;
    }

    apActor->MoveTo(pCell, acPosition);
}

void CharacterService::ProcessNewEntity(entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    auto& formIdComponent = m_world.get<FormIdComponent>(aEntity);

    Actor* const pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
    if (!pActor)
    {
        spdlog::warn(__FUNCTION__ ": actor for new entity not found, form id: {:X}", formIdComponent.Id);
        return;
    }

    if (auto* pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity); pRemoteComponent)
    {
        // TODO(cosideci): don't just take all actors (i.e. from other parties),
        // maybe check it server side, add a variable to the request.
        if (m_world.GetPartyService().IsLeader() && !pActor->IsTemporary() && !pActor->IsMount())
        {
            spdlog::info("Sending ownership claim for actor {:X} with server id {:X}", pActor->formID, pRemoteComponent->Id);

            RequestOwnership(pActor->formID, pRemoteComponent->Id, aEntity);
        }
        else
            spdlog::info("New entity remotely managed, form id: {:X}, server id: {:X}", pActor->formID, pRemoteComponent->Id);

        return;
    }

    if (m_world.any_of<RemoteComponent, LocalComponent, WaitingForAssignmentComponent>(aEntity))
        return;

    CacheSystem::Setup(World::Get(), aEntity, pActor);

    RequestServerAssignment(aEntity);
}

void CharacterService::RequestServerAssignment(const entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    static uint32_t sCookieSeed = 0;

    const auto& formIdComponent = m_world.get<FormIdComponent>(aEntity);

    auto* pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
    if (!pActor)
        return;

    TESNPC* pNpc = Cast<TESNPC>(pActor->baseForm);
    if (!pNpc)
        return;

    AssignCharacterRequest message{};

    message.Cookie = sCookieSeed;

    if (!m_world.GetModSystem().GetServerModId(formIdComponent.Id, message.ReferenceId))
    {
        spdlog::error("Server reference id not found for form id {:X}", formIdComponent.Id);
        return;
    }

    if (!m_world.GetModSystem().GetServerModId(pActor->parentCell->formID, message.CellId))
    {
        spdlog::error("Server cell id not found for cell id {:X}", pActor->parentCell->formID);
        return;
    }

    if (const auto pWorldSpace = pActor->GetWorldSpace())
    {
        if (!m_world.GetModSystem().GetServerModId(pWorldSpace->formID, message.WorldSpaceId))
            return;
    }

    message.Position = pActor->position;
    message.Rotation.x = pActor->rotation.x;
    message.Rotation.y = pActor->rotation.z;

    // Serialize the base form
    const auto isPlayer = (formIdComponent.Id == 0x14);
    const auto isTemporary = pActor->formID >= 0xFF000000;

    if (isPlayer)
    {
        pNpc->MarkChanged(0x2000800);
    }

    const auto changeFlags = pNpc->GetChangeFlags();

    if (isPlayer || changeFlags != 0)
    {
        message.ChangeFlags = changeFlags;
        pNpc->Serialize(&message.AppearanceBuffer);
    }

    if (isPlayer)
    {
        auto& entries = message.FaceTints.Entries;

        const auto& tints = PlayerCharacter::Get()->GetTints();

        entries.resize(tints.length);

        for (auto i = 0u; i < tints.length; ++i)
        {
            entries[i].Alpha = tints[i]->alpha;
            entries[i].Color = tints[i]->color;
            entries[i].Type = tints[i]->type;

            if (tints[i]->texture)
                entries[i].Name = tints[i]->texture->name.AsAscii();
        }
    }

    if (isPlayer)
    {
        auto& questLog = message.QuestContent.Entries;
        auto& modSystem = m_world.GetModSystem();

        for (const auto& objective : PlayerCharacter::Get()->objectives)
        {
            auto* pQuest = objective.instance->quest;
            if (!pQuest)
                continue;

            if (!QuestService::IsNonSyncableQuest(pQuest))
            {
                GameId id{};

                if (modSystem.GetServerModId(pQuest->formID, id))
                {
                    auto& entry = questLog.emplace_back();
                    entry.Stage = pQuest->currentStage;
                    entry.Id = id;
                }
            }
        }

        // remove duplicates
        const auto ip = std::unique(questLog.begin(), questLog.end());
        questLog.resize(std::distance(questLog.begin(), ip));
    }

    message.CurrentActorData = BuildActorData(pActor);

    message.FactionsContent = pActor->GetFactions();
    message.IsDragon = pActor->IsDragon();
    message.IsMount = pActor->IsMount();
    message.IsPlayerSummon = pActor->GetCommandingActor() && pActor->GetCommandingActor()->formID == 0x14;

    if (pNpc->IsTemporary())
    {
        // The chain is derived from the live actor and cannot go stale; the
        // resolver map is keyed by temp ids the engine recycles, and cell
        // attach bypasses the hook, so a map hit may describe a previous
        // occupant of this id. Only named leveled NPCs, whose chain hides
        // the pick, fall back to the map.
        uint32_t pickFormId = 0;
        if (TESNPC* pChainPick = pNpc->GetLeveledPick())
            pickFormId = pChainPick->formID;

        if (pickFormId == 0)
            pickFormId = TESNPC::GetLeveledPickFormId(pNpc->formID);

        if (pickFormId != 0)
        {
            if (m_world.GetModSystem().GetServerModId(pickFormId, message.LeveledNpcPickId))
                spdlog::info("Captured leveled NPC pick {:X} for actor {:X} (temp base {:X})", pickFormId, pActor->formID, pNpc->formID);
            else
                spdlog::warn("Leveled NPC pick {:X} has no server id, identity sync skipped", pickFormId);
        }
        else
            spdlog::info("No leveled pick recoverable for temp base {:X} (actor {:X}), identity sync unavailable", pNpc->formID, pActor->formID);

        pNpc = pNpc->GetTemplateBase();
    }

    if (isTemporary)
    {
        if (pNpc && !m_world.GetModSystem().GetServerModId(pNpc->formID, message.FormId))
        {
            spdlog::error("Server NPC form id not found for form id {:X}", pNpc->formID);
            return;
        }
    }

    // Serialize actions
    auto* const pExtension = pActor->GetExtension();

    message.LatestAction = pExtension->LatestAnimation;
    pActor->SaveAnimationVariables(message.LatestAction.Variables);

    spdlog::info("Request id: {:X}, cookie: {:X}, entity: {:X}", formIdComponent.Id, sCookieSeed, to_integral(aEntity));

    if (m_transport.Send(message))
    {
        m_world.emplace<WaitingForAssignmentComponent>(aEntity, sCookieSeed);

        sCookieSeed++;
    }
}

void CharacterService::CancelServerAssignment(const entt::entity aEntity, const uint32_t aFormId) const noexcept
{
    if (m_world.all_of<RemoteComponent>(aEntity))
    {
        Actor* pActor = Cast<Actor>(TESForm::GetById(aFormId));

        if (pActor)
        {
            if (pActor->IsTemporary())
            {
                spdlog::info("Temporary Remote Deleted {:X}", aFormId);
                pActor->Delete();
            }
            else
            {
                pActor->GetExtension()->SetRemote(false);
            }
        }

        DeleteRemoteEntityComponents(aEntity);

        return;
    }

    // Keep the cookie until the server response arrives so awarded ownership can be relinquished.
    if (m_world.all_of<WaitingForAssignmentComponent>(aEntity))
    {
        auto& waitingComponent = m_world.get<WaitingForAssignmentComponent>(aEntity);
        waitingComponent.Cancelled = true;
        return;
    }

    if (m_world.all_of<LocalComponent>(aEntity))
    {
        auto& localComponent = m_world.get<LocalComponent>(aEntity);

        RequestOwnershipTransfer request{};
        request.ServerId = localComponent.Id;
        request.OwnershipEpoch = localComponent.OwnershipEpoch;
        request.Reason = OwnershipReleaseReason::Relinquish;

        if (Actor* pActor = Cast<Actor>(TESForm::GetById(aFormId)))
        {
            if (!pActor->IsTemporary())
            {
                auto& modSystem = m_world.GetModSystem();

                if (TESWorldSpace* pWorldSpace = pActor->GetWorldSpace())
                {
                    if (!modSystem.GetServerModId(pWorldSpace->formID, request.WorldSpaceId))
                        spdlog::error("World space id not found, despite having a world space, {:X}", pWorldSpace->formID);
                }

                if (TESObjectCELL* pCell = pActor->GetParentCell())
                {
                    if (!modSystem.GetServerModId(pCell->formID, request.CellId))
                        spdlog::error("Cell id not found, despite having a cell, {:X}", pCell->formID);
                }

                request.Position = pActor->position;
            }
        }

        spdlog::info(
            "Transferring ownership of local actor, server id: {:X}, epoch: {}, worldspace: {:X}, cell: {:X}, position: "
            "({}, {}, {})",
            request.ServerId, request.OwnershipEpoch, request.WorldSpaceId.BaseId, request.CellId.BaseId, request.Position.x, request.Position.y, request.Position.z);

        m_transport.Send(request);

        m_world.remove<LocalAnimationComponent, LocalComponent>(aEntity);
    }
}

Actor* CharacterService::CreateCharacterForEntity(entt::entity aEntity) const noexcept
{
    auto* pWaitingFor3D = m_world.try_get<WaitingFor3D>(aEntity);
    auto* pInterpolationComponent = m_world.try_get<InterpolationComponent>(aEntity);

    if (!pWaitingFor3D || !pInterpolationComponent)
    {
        spdlog::error(__FUNCTION__ ": could not find WaitingFor3D or InterpolationComponent");
        return nullptr;
    }

    auto& acMessage = pWaitingFor3D->SpawnRequest;

    Actor* pActor = nullptr;

    // Custom forms
    if (acMessage.FormId == GameId{})
    {
        TESNPC* pNpc = nullptr;

        if (acMessage.BaseId != GameId{})
        {
            // Prefer the owner's resolved leveled pick over the lossy template base
            GameId baseId = acMessage.BaseId;
            uint32_t npcId = World::Get().GetModSystem().GetGameId(baseId);
            if (acMessage.LeveledNpcPickId != GameId{})
            {
                if (const uint32_t cPickNpcId = World::Get().GetModSystem().GetGameId(acMessage.LeveledNpcPickId))
                {
                    baseId = acMessage.LeveledNpcPickId;
                    npcId = cPickNpcId;
                }
            }

            if (npcId == 0)
            {
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod");
                return nullptr;
            }

            pNpc = Cast<TESNPC>(TESForm::GetById(npcId));
            pNpc->Deserialize(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
        }
        else
        {
            pNpc = TESNPC::Create(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
            FaceGenSystem::Setup(m_world, aEntity, acMessage.FaceTints);
        }

        pActor = Actor::Create(pNpc);
    }

    auto& remoteComponent = m_world.get<RemoteComponent>(aEntity);

    if (!pActor)
    {
        spdlog::error(__FUNCTION__ ": could not spawn actor for remote server id {:X}.", remoteComponent.Id);
        return nullptr;
    }

    pActor->GetExtension()->SetRemote(true);
    pActor->rotation.x = acMessage.Rotation.x;
    pActor->rotation.z = acMessage.Rotation.y;
    pActor->MoveTo(PlayerCharacter::Get()->parentCell, pInterpolationComponent->Position);
    pActor->SetActorValues(acMessage.InitialActorValues);

    pActor->GetExtension()->SetPlayer(acMessage.IsPlayer);
    if (acMessage.IsPlayer)
    {
        pActor->SetIgnoreFriendlyHit(true);
        pActor->SetPlayerRespawnMode();
        m_world.emplace_or_replace<PlayerComponent>(aEntity, acMessage.PlayerId);
    }

    if (pActor->IsDead() != acMessage.IsDead)
        acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

    spdlog::info("Spawned character for entity, server id: {:X}", remoteComponent.Id);

    return pActor;
}

ActorData CharacterService::BuildActorData(Actor* apActor) const noexcept
{
    ActorData actorData{};
    actorData.InitialActorValues = apActor->GetEssentialActorValues();
    actorData.InitialInventory = apActor->GetActorInventory();
    actorData.IsDead = apActor->IsDead();
    actorData.IsWeaponDrawn = apActor->actorState.IsWeaponFullyDrawn();

    return actorData;
}

// A static base still templating onto a leveled list is the placed shell:
// the local engine has not rolled this actor yet. Shells have no model of
// their own - such actors render invisible or headless until conformed.
static bool IsUnresolvedLeveledShell(const TESNPC* apBase) noexcept
{
    if (!apBase || apBase->IsTemporary())
        return false;

    const TESNPC* pTemplate = apBase->npcTemplate;
    return pTemplate && pTemplate->formType == FormType::LeveledCharacter;
}

void CharacterService::ApplyLeveledNpcPick(Actor* apActor, const GameId& acPickId) const noexcept
{
    if (acPickId == GameId{})
        return;

    TESNPC* pBase = Cast<TESNPC>(apActor->baseForm);
    if (!pBase)
        return;

    if (!pBase->IsTemporary())
    {
        // Conforming a shell is exactly what resolution would have done; any
        // other static base is an already conformed actor.
        if (!IsUnresolvedLeveledShell(pBase))
        {
            spdlog::info("Leveled pick {:x}:{:x} received for actor {:X} whose base is not a leveled temp, skipping", acPickId.ModId, acPickId.BaseId, apActor->formID);
            return;
        }

        spdlog::info("Actor {:X} still carries unresolved shell base {:X}, conforming to owner's pick", apActor->formID, pBase->formID);
    }

    const uint32_t cPickId = World::Get().GetModSystem().GetGameId(acPickId);
    if (cPickId == 0)
    {
        spdlog::warn("Leveled NPC pick {:X}:{:X} not resolvable, possibly missing mod, keeping local pick", acPickId.ModId, acPickId.BaseId);
        return;
    }

    TESNPC* pPick = Cast<TESNPC>(TESForm::GetById(cPickId));
    if (!pPick)
    {
        spdlog::warn("Leveled NPC pick {:X} is not an NPC, keeping local pick", cPickId);
        return;
    }

    // Chain first for the same staleness reason as the capture side
    uint32_t localPickId = 0;
    if (TESNPC* pLocalPick = pBase->GetLeveledPick())
        localPickId = pLocalPick->formID;

    if (localPickId == 0)
        localPickId = TESNPC::GetLeveledPickFormId(pBase->formID);

    if (localPickId == cPickId)
    {
        spdlog::info("Leveled actor {:X} already matches owner's pick {:X}", apActor->formID, cPickId);
        return;
    }

    spdlog::info("Conforming leveled actor {:X} (temp base {:X}, local pick {:X}) to owner's pick {:X}", apActor->formID, pBase->formID, localPickId, cPickId);

    // Never mutate the reference here: this runs from message handlers, while
    // the cell attach may still own the reference, and queueing to the runner
    // from a drained task re-locks the drain mutex (UB). The service update
    // tick applies pending conforms once the world has settled.
    m_pendingLeveledConforms[apActor->formID] = {cPickId, false};
}

void CharacterService::ProcessLeveledConforms() noexcept
{
    if (m_pendingLeveledConforms.empty())
        return;

    // Never touch references while the loading screen is up - the cell attach
    // owns them and mutating mid-stream crashes the loader
    UI* pUI = UI::Get();
    if (pUI && pUI->GetMenuOpen(BSFixedString("Loading Menu")))
        return;

    for (auto it = m_pendingLeveledConforms.begin(); it != m_pendingLeveledConforms.end();)
    {
        LeveledConformData& conform = it.value();

        Actor* pActor = Cast<Actor>(TESForm::GetById(it->first));
        TESNPC* pPick = Cast<TESNPC>(TESForm::GetById(conform.PickFormId));
        if (!pActor || !pPick)
        {
            it = m_pendingLeveledConforms.erase(it);
            continue;
        }

        if (conform.Disabled)
        {
            // Teardown ran last tick; rebuild the 3D from the pick
            pActor->baseForm = pPick;
            pActor->EnableImpl();

            // The animation sync caches the graph descriptor per actor. A pick that
            // crosses animation projects (rabbit -> fox) keeps the old project's
            // variable indices, and every remote update then scribbles the owner's
            // values through them into the new graph's variable set - the OOB
            // variable-index crash. Zero it so the next sync tick recomputes it
            // from the rebuilt graph, like the werewolf/vampire lord transforms do.
            pActor->GetExtension()->GraphDescriptorHash = 0;

            spdlog::info("Re-enabled conformed leveled actor {:X}, base {:X}", it->first, conform.PickFormId);
            it = m_pendingLeveledConforms.erase(it);
            continue;
        }

        if (!pActor->loadedState && !IsUnresolvedLeveledShell(Cast<TESNPC>(pActor->baseForm)))
        {
            // Distant actors stream their 3D in whenever the player approaches -
            // possibly minutes later. Stay pending until then; a newer pick
            // overwrites this entry and a disconnect clears the map. Shell-based
            // actors are exempt: they have no model to load until conformed.
            ++it;
            continue;
        }

        pActor->DisableImpl();
        conform.Disabled = true;
        ++it;
    }
}

void CharacterService::RunLocalUpdates() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 100ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    ClientReferencesMoveRequest message;
    message.Tick = m_transport.GetClock().GetCurrentTick();

    auto animatedLocalView = m_world.view<LocalComponent, LocalAnimationComponent, FormIdComponent>();

    for (auto entity : animatedLocalView)
    {
        auto& localComponent = animatedLocalView.get<LocalComponent>(entity);
        auto& animationComponent = animatedLocalView.get<LocalAnimationComponent>(entity);
        auto& formIdComponent = animatedLocalView.get<FormIdComponent>(entity);

        AnimationSystem::Serialize(m_world, message, localComponent, animationComponent, formIdComponent);
    }

    m_transport.Send(message);
}

void CharacterService::RunRemoteUpdates() noexcept
{
    // Delay by 300ms to let the interpolation system accumulate interpolation points
    const auto tick = m_transport.GetClock().GetCurrentTick() - 300;

    // Interpolation has to keep running even if the actor is not in view, otherwise we will never know if we need to spawn it
    auto interpolatedEntities = m_world.view<RemoteComponent, InterpolationComponent>();

    for (auto entity : interpolatedEntities)
    {
        auto* pFormIdComponent = m_world.try_get<FormIdComponent>(entity);
        auto& interpolationComponent = interpolatedEntities.get<InterpolationComponent>(entity);

        Actor* pActor = nullptr;
        if (pFormIdComponent)
        {
            auto* pForm = TESForm::GetById(pFormIdComponent->Id);
            pActor = Cast<Actor>(pForm);
        }

        InterpolationSystem::Update(pActor, interpolationComponent, tick);
    }

    auto animatedView = m_world.view<RemoteComponent, RemoteAnimationComponent, FormIdComponent>();

    for (auto entity : animatedView)
    {
        auto& animationComponent = animatedView.get<RemoteAnimationComponent>(entity);
        auto& formIdComponent = animatedView.get<FormIdComponent>(entity);

        auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = Cast<Actor>(pForm);
        if (!pActor)
            continue;

        AnimationSystem::Update(m_world, pActor, animationComponent, tick);
    }

    auto facegenView = m_world.view<FormIdComponent, FaceGenComponent>();

    for (auto entity : facegenView)
    {
        auto& formIdComponent = facegenView.get<FormIdComponent>(entity);
        auto& faceGenComponent = facegenView.get<FaceGenComponent>(entity);

        const auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = Cast<Actor>(pForm);
        if (!pActor)
            continue;

        FaceGenSystem::Update(m_world, pActor, faceGenComponent);
    }

    auto waitingView = m_world.view<FormIdComponent, WaitingFor3D>();

    Vector<entt::entity> readyEntities;
    for (auto entity : waitingView)
    {
        auto& formIdComponent = waitingView.get<FormIdComponent>(entity);
        auto& waitingFor3D = waitingView.get<WaitingFor3D>(entity);

        Actor* pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
        if (!pActor || !pActor->GetNiNode())
            continue;

        // By now, the actor has materialized in the world and is ready for further setup

        pActor->SetActorInventory(waitingFor3D.SpawnRequest.InventoryContent);
        pActor->SetFactions(waitingFor3D.SpawnRequest.FactionsContent);

        if (!waitingFor3D.SpawnRequest.ActionsToReplay.Actions.empty())
        {
            pActor->LoadAnimationVariables(waitingFor3D.SpawnRequest.ActionsToReplay.Actions[0].Variables);
        }

        m_weaponDrawUpdates[pActor->formID] = {waitingFor3D.SpawnRequest.IsWeaponDrawn};

        if (pActor->IsDead() != waitingFor3D.SpawnRequest.IsDead)
            waitingFor3D.SpawnRequest.IsDead ? pActor->Kill() : pActor->Respawn();

        if (pActor->IsVampireLord())
            pActor->FixVampireLordModel();

        readyEntities.push_back(entity);

        spdlog::info("Applied 3D for actor, form id: {:X}", pActor->formID);
    }

    for (auto entity : readyEntities)
    {
        m_world.remove<WaitingFor3D>(entity);

        // Reprocess the remote actor now that an ownership grant can be accepted without immediately declining it.
        ProcessNewEntity(entity);
    }
}

void CharacterService::RunFactionsUpdates() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 2000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    RequestFactionsChanges message;

    auto factionedActors = m_world.view<LocalComponent, CacheComponent, FormIdComponent>();
    for (auto entity : factionedActors)
    {
        auto& formIdComponent = factionedActors.get<FormIdComponent>(entity);
        auto& localComponent = factionedActors.get<LocalComponent>(entity);
        auto& cacheComponent = factionedActors.get<CacheComponent>(entity);

        const auto* pForm = TESForm::GetById(formIdComponent.Id);
        const auto* pActor = Cast<Actor>(pForm);
        if (!pActor)
            continue;

        // Check if cached factions and current factions are identical
        auto factions = pActor->GetFactions();

        if (cacheComponent.FactionsContent == factions)
            continue;

        cacheComponent.FactionsContent = factions;

        // If not send the current factions and replace the cached factions
        message.Changes[localComponent.Id] = factions;
    }

    if (!message.Changes.empty())
        m_transport.Send(message);
}

void CharacterService::RunSpawnUpdates() const noexcept
{
    auto invisibleView = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, WaitingFor3D>(entt::exclude<FormIdComponent>);
    Vector<entt::entity> entities(invisibleView.begin(), invisibleView.end());

    for (const auto entity : entities)
    {
        auto& remoteComponent = m_world.get<RemoteComponent>(entity);
        auto& interpolationComponent = m_world.get<InterpolationComponent>(entity);

        if (const auto pWorldSpace = PlayerCharacter::Get()->GetWorldSpace())
        {
            float characterX = interpolationComponent.Position.x;
            float characterY = interpolationComponent.Position.y;
            const auto characterCoords = GridCellCoords::CalculateGridCellCoords(characterX, characterY);
            const TES* pTES = TES::Get();
            const auto playerCoords = GridCellCoords(pTES->centerGridX, pTES->centerGridY);

            // TODO(cosideci): IsDragon probably shouldn't be straight up false here.
            if (GridCellCoords::IsCellInGridCell(characterCoords, playerCoords, false))
            {
                auto* pActor = Cast<Actor>(TESForm::GetById(remoteComponent.CachedRefId));
                if (!pActor)
                {
                    pActor = CreateCharacterForEntity(entity);
                    if (!pActor)
                        continue;

                    remoteComponent.CachedRefId = pActor->formID;
                }

                pActor->MoveTo(PlayerCharacter::Get()->parentCell, interpolationComponent.Position);
            }
        }
    }
}

void CharacterService::RunExperienceUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (m_cachedExperience == 0.f)
        return;

    if (!World::Get().GetPartyService().IsInParty())
        return;

    SyncExperienceRequest message;
    message.Experience = m_cachedExperience;

    m_cachedExperience = 0.f;

    m_transport.Send(message);

    spdlog::debug("Sending over experience {}", message.Experience);
}

void CharacterService::ApplyCachedWeaponDraws(const UpdateEvent& acUpdateEvent) noexcept
{
    std::vector<uint32_t> toRemove{};

    for (auto& [cId, _] : m_weaponDrawUpdates)
    {
        auto& data = m_weaponDrawUpdates[cId];

        data.m_timer += acUpdateEvent.Delta;

        // Remote actors get 2 passes because Skyrim's weapon drawing is the most finnicky thing in existence.
        double maxTime = data.m_isFirstPass ? 0.5 : 2.0;
        if (data.m_timer <= maxTime)
            continue;

        Actor* pActor = Cast<Actor>(TESForm::GetById(cId));
        if (!pActor || !pActor->GetExtension()->IsRemote())
        {
            toRemove.push_back(cId);
            continue;
        }

        pActor->SetWeaponDrawnEx(data.m_drawWeapon);

        if (!data.m_isFirstPass)
            toRemove.push_back(cId);

        data.m_isFirstPass = false;
    }

    for (uint32_t id : toRemove)
        m_weaponDrawUpdates.erase(id);
}
