#include <TiltedOnlinePCH.h>

#include "Forms/TESObjectCELL.h"
#include "Forms/TESWorldSpace.h"
#include "Games/Misc/UI.h"
#include "Services/PapyrusService.h"


#include <Services/CharacterService.h>
#include <Services/QuestService.h>
#include <Services/TransportService.h>
#include <Services/QuestService.h>

#include <Games/References.h>

#include <ExtraData/ExtraLeveledCreature.h>
#include <Forms/TESNPC.h>
#include <Forms/TESQuest.h>
#include <ExtraData/ExtraLeveledCreature.h>

#include <Components.h>

#include <Systems/InterpolationSystem.h>
#include <Systems/AnimationSystem.h>
#include <Systems/CacheSystem.h>
#include <Systems/FaceGenSystem.h>
#include <Systems/CacheSystem.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/EquipmentChangeEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/SpellCastEvent.h>
#include <Events/InterruptCastEvent.h>
#include <Events/AddTargetEvent.h>
#include <Events/ProjectileLaunchedEvent.h>

#include <Structs/ActionEvent.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/RequestSpawnData.h>
#include <Messages/NotifySpawnData.h>
#include <Messages/RequestOwnershipTransfer.h>
#include <Messages/NotifyOwnershipTransfer.h>
#include <Messages/RequestOwnershipClaim.h>
#include <Messages/SpellCastRequest.h>
#include <Messages/NotifySpellCast.h>
#include <Messages/ProjectileLaunchRequest.h>
#include <Messages/NotifyProjectileLaunch.h>
#include <Messages/InterruptCastRequest.h>
#include <Messages/NotifyInterruptCast.h>
#include <Messages/AddTargetRequest.h>
#include <Messages/NotifyAddTarget.h>

#include <World.h>
#include <Games/TES.h>

#include <Projectiles/Projectile.h>
#include <Forms/TESObjectWEAP.h>
#include <Forms/TESAmmo.h>

CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_formIdAddedConnection = m_world.on_construct<FormIdComponent>().connect<&CharacterService::OnFormIdComponentAdded>(this);
    m_formIdRemovedConnection = m_world.on_destroy<FormIdComponent>().connect<&CharacterService::OnFormIdComponentRemoved>(this);
    m_formIdRemovedConnection =
        m_world.on_destroy<FormIdComponent>().connect<&CharacterService::OnFormIdComponentRemoved>(this);

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
    m_remoteSpawnDataReceivedConnection = m_dispatcher.sink<NotifySpawnData>().connect<&CharacterService::OnRemoteSpawnDataReceived>(this);
    m_spellCastEventConnection = m_dispatcher.sink<SpellCastEvent>().connect<&CharacterService::OnSpellCastEvent>(this);
    m_notifySpellCastConnection = m_dispatcher.sink<NotifySpellCast>().connect<&CharacterService::OnNotifySpellCast>(this);
    m_interruptCastEventConnection = m_dispatcher.sink<InterruptCastEvent>().connect<&CharacterService::OnInterruptCast>(this);
    m_notifyInterruptCastConnection = m_dispatcher.sink<NotifyInterruptCast>().connect<&CharacterService::OnNotifyInterruptCast>(this);
    m_addTargetEventConnection = m_dispatcher.sink<AddTargetEvent>().connect<&CharacterService::OnAddTargetEvent>(this);
    m_notifyAddTargetConnection = m_dispatcher.sink<NotifyAddTarget>().connect<&CharacterService::OnNotifyAddTarget>(this);
    m_projectileLaunchedConnection = m_dispatcher.sink<ProjectileLaunchedEvent>().connect<&CharacterService::OnProjectileLaunchedEvent>(this);
    m_projectileLaunchConnection = m_dispatcher.sink<NotifyProjectileLaunch>().connect<&CharacterService::OnNotifyProjectileLaunch>(this);
}

void CharacterService::OnFormIdComponentAdded(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    // Security check
    auto* const pForm = TESForm::GetById(formIdComponent.Id);
    auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);
    if (!pActor)
        return;

    if (pActor->GetExtension()->IsRemote())
    {
        spdlog::info("New entity remotely managed? {:X}", pActor->formID);
    }

    if (auto* pRemoteComponent = aRegistry.try_get<RemoteComponent>(aEntity); pRemoteComponent)
    {
        RequestSpawnData requestSpawnData;
        requestSpawnData.Id = pRemoteComponent->Id;
        m_transport.Send(requestSpawnData);
    }

    if (aRegistry.any_of<RemoteComponent, LocalComponent, WaitingForAssignmentComponent>(aEntity))
        return;

    CacheSystem::Setup(World::Get(), aEntity, pActor);

    auto* const pNpc = RTTI_CAST(pActor->baseForm, TESForm, TESNPC);

    auto* pExtra = static_cast<ExtraLeveledCreature*>(pActor->GetExtraDataList()->GetByType(ExtraData::LeveledCreature));

    auto* const pOwner = pNpc->actorData.owner;
    if (pOwner)
    {
        //    spdlog::info("\tOwner: type {}, id {:X}", static_cast<uint32_t>(pOwner->formType), pOwner->formID);
    }

    if(pNpc)
    {
        RequestServerAssignment(aRegistry, aEntity);
    }
}

void CharacterService::OnFormIdComponentRemoved(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    CancelServerAssignment(aRegistry, aEntity, formIdComponent.Id);
}

void CharacterService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunSpawnUpdates();
    RunLocalUpdates();
    RunFactionsUpdates();
    RunRemoteUpdates();
}

void CharacterService::OnConnected(const ConnectedEvent& acConnectedEvent) const noexcept
{
    //m_world.clear<WaitingForAssignmentComponent, LocalComponent, RemoteComponent>();

    // Go through all the forms that were previously detected
    auto view = m_world.view<FormIdComponent>();
    for (auto entity : view)
    {
        OnFormIdComponentAdded(m_world, entity);
    }
}

void CharacterService::OnDisconnected(const DisconnectedEvent& acDisconnectedEvent) const noexcept
{
    auto remoteView = m_world.view<FormIdComponent, RemoteComponent>();
    for (auto entity : remoteView)
    {
        auto& formIdComponent = remoteView.get<FormIdComponent>(entity);

        auto pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (pActor)
            pActor->GetExtension()->SetRemote(false);
    }

    m_world.clear<WaitingForAssignmentComponent, LocalComponent, RemoteComponent>();
}

void CharacterService::OnAssignCharacter(const AssignCharacterResponse& acMessage) const noexcept
{
    spdlog::info("Received for cookie {:X}", acMessage.Cookie);

    auto view = m_world.view<WaitingForAssignmentComponent>();
    const auto itor = std::find_if(std::begin(view), std::end(view), [view, cookie = acMessage.Cookie](auto entity)
    {
        return view.get<WaitingForAssignmentComponent>(entity).Cookie == cookie;
    });

    if (itor == std::end(view))
    {
        spdlog::warn("Never found requested cookie: {}", acMessage.Cookie);
        return;
    }

    const auto cEntity = *itor;

    const auto& formIdComponent = m_world.get<FormIdComponent>(cEntity);

    m_world.remove<WaitingForAssignmentComponent>(cEntity);

    if (m_world.any_of<LocalComponent, RemoteComponent>(cEntity))
    {
        auto* const pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
            return;

        if (pActor->IsDead() != acMessage.IsDead)
            acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

        return;
    }

    if (acMessage.Owner)
    {
        m_world.emplace<LocalComponent>(cEntity, acMessage.ServerId);
        m_world.emplace<LocalAnimationComponent>(cEntity);
    }
    else
    {
        auto* const pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
        {
            m_world.destroy(cEntity);
            return;
        }

        m_world.emplace_or_replace<RemoteComponent>(cEntity, acMessage.ServerId, formIdComponent.Id);

        pActor->GetExtension()->SetRemote(true);

        InterpolationSystem::Setup(m_world, cEntity);
        AnimationSystem::Setup(m_world, cEntity);

        pActor->SetActorValues(acMessage.AllActorValues);

        if (pActor->IsDead() != acMessage.IsDead)
            acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

        const auto cCellId = World::Get().GetModSystem().GetGameId(acMessage.CellId);
        const auto* pCellForm = TESForm::GetById(cCellId);
        auto* pCell = RTTI_CAST(pCellForm, TESForm, TESObjectCELL);
        if (pCell)
            pActor->MoveTo(pCell, acMessage.Position);
    }
}

void CharacterService::OnCharacterSpawn(const CharacterSpawnRequest& acMessage) const noexcept
{
    auto remoteView = m_world.view<RemoteComponent>();
    const auto remoteItor = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ServerId](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

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
            const auto cNpcId = World::Get().GetModSystem().GetGameId(acMessage.BaseId);
            if(cNpcId == 0)
            {
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod");
                return;
            }

            pNpc = RTTI_CAST(TESForm::GetById(cNpcId), TESForm, TESNPC);
            pNpc->Deserialize(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
        }
        else
        {
            pNpc = TESNPC::Create(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
            FaceGenSystem::Setup(m_world, *entity, acMessage.FaceTints);
        }

        pActor = Actor::Create(pNpc);
    }
    else
    {
        const auto cActorId = World::Get().GetModSystem().GetGameId(acMessage.FormId);

        auto* const pForm = TESForm::GetById(cActorId);
        pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
        {
            spdlog::error("Failed to retrieve Actor {:X}, it will not be spawned, possibly missing mod", cActorId);
            spdlog::error("\tForm : {:X}", pForm ? pForm->formID : 0);
            return;
        }

        const auto view = m_world.view<FormIdComponent>();
        const auto itor = std::find_if(std::begin(view), std::end(view), [cActorId, view](entt::entity entity) {
            return view.get<FormIdComponent>(entity).Id == cActorId;
        });

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

    pActor->GetExtension()->SetRemote(true);
    pActor->rotation.x = acMessage.Rotation.x;
    pActor->rotation.z = acMessage.Rotation.y;
    pActor->MoveTo(PlayerCharacter::Get()->parentCell, acMessage.Position);
    pActor->SetActorValues(acMessage.InitialActorValues);
    pActor->GetExtension()->SetPlayer(acMessage.IsPlayer);

    if (pActor->IsDead() != acMessage.IsDead)
        acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

    auto& remoteComponent = m_world.emplace_or_replace<RemoteComponent>(*entity, acMessage.ServerId, pActor->formID);
    remoteComponent.SpawnRequest = acMessage;

    auto& interpolationComponent = InterpolationSystem::Setup(m_world, *entity);
    interpolationComponent.Position = acMessage.Position;

    AnimationSystem::Setup(m_world, *entity);

    m_world.emplace<WaitingFor3D>(*entity);

    auto& remoteAnimationComponent = m_world.get<RemoteAnimationComponent>(*entity);
    remoteAnimationComponent.TimePoints.push_back(acMessage.LatestAction);
}

void CharacterService::OnRemoteSpawnDataReceived(const NotifySpawnData& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent>();

    const auto id = acEvent.Id;

    const auto itor = std::find_if(std::begin(view), std::end(view), [view, id](auto entity) {
        const auto& remoteComponent = view.get<RemoteComponent>(entity);

        return remoteComponent.Id == id;
    });

    if (itor != std::end(view))
    {
        auto& remoteComponent = view.get<RemoteComponent>(*itor);
        remoteComponent.SpawnRequest.InitialActorValues = acEvent.InitialActorValues;
        remoteComponent.SpawnRequest.InventoryContent = acEvent.InitialInventory;
        remoteComponent.SpawnRequest.IsDead = acEvent.IsDead;

        auto& formIdComponent = view.get<FormIdComponent>(*itor);
        auto* const pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
            return;

        pActor->SetActorValues(remoteComponent.SpawnRequest.InitialActorValues);
        pActor->SetInventory(remoteComponent.SpawnRequest.InventoryContent);

        if (pActor->IsDead() != acEvent.IsDead)
            acEvent.IsDead ? pActor->Kill() : pActor->Respawn();
    }
}

void CharacterService::OnReferencesMoveRequest(const ServerReferencesMoveRequest& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>();

    for (const auto& [serverId, update] : acMessage.Updates)
    {
        auto itor = std::find_if(std::begin(view), std::end(view), [serverId = serverId, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == serverId;
        });

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

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acActionEvent.ActorId, view](entt::entity entity)
    {
            return view.get<FormIdComponent>(entity).Id == id;
        });

    if(itor != std::end(view))
    {
        auto& localComponent = view.get<LocalAnimationComponent>(*itor);

        localComponent.Append(acActionEvent);
    }
}

void CharacterService::OnFactionsChanges(const NotifyFactionsChanges& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent, CacheComponent>();

    for (const auto& [id, factions] : acEvent.Changes)
    {
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = id, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == id;
        });

        if (itor != std::end(view))
        {
            auto& formIdComponent = view.get<FormIdComponent>(*itor);

            auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
            if (!pActor)
                return;

            auto& cacheComponent = view.get<CacheComponent>(*itor);
            cacheComponent.FactionsContent = factions;

            pActor->SetFactions(cacheComponent.FactionsContent);
        }
    }
}

void CharacterService::OnOwnershipTransfer(const NotifyOwnershipTransfer& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [&acMessage, &view](auto entity) {
        return view.get<RemoteComponent>(entity).Id == acMessage.ServerId;
    });

    if (itor != std::end(view))
    {
        auto& formIdComponent = view.get<FormIdComponent>(*itor);

        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (pActor)
        {
            pActor->GetExtension()->SetRemote(false);

            m_world.emplace<LocalComponent>(*itor, acMessage.ServerId);
            m_world.emplace<LocalAnimationComponent>(*itor);
            m_world.remove<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, 
                                     FaceGenComponent, CacheComponent, WaitingFor3D>(*itor);

            RequestOwnershipClaim request;
            request.ServerId = acMessage.ServerId;

            m_transport.Send(request);
            spdlog::info("Ownership claimed {:X}", request.ServerId);

            return;
        }
    }

    spdlog::warn("Actor for ownership transfer not found {:X}", acMessage.ServerId);

    RequestOwnershipTransfer request;
    request.ServerId = acMessage.ServerId;

    m_transport.Send(request);
}

void CharacterService::OnRemoveCharacter(const NotifyRemoveCharacter& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acMessage.ServerId, view](entt::entity entity) {
            return view.get<RemoteComponent>(entity).Id == id;
        });

    if (itor != std::end(view))
    {
        if (auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*itor))
        {
            const auto pActor = RTTI_CAST(TESForm::GetById(pFormIdComponent->Id), TESForm, Actor);
            if (pActor && ((pActor->formID & 0xFF000000) == 0xFF000000))
            {
                spdlog::info("\tDeleting {:X}", pFormIdComponent->Id);
                pActor->Delete();
            }
        }

        m_world.remove<RemoteComponent, RemoteAnimationComponent, InterpolationComponent>(*itor);
    }
}

void CharacterService::RequestServerAssignment(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    static uint32_t sCookieSeed = 0;

    const auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    auto* pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
    if (!pActor)
        return;

    auto* const pNpc = RTTI_CAST(pActor->baseForm, TESForm, TESNPC);
    if (!pNpc)
        return;

    uint32_t baseId = 0;
    uint32_t modId = 0;
    if (!m_world.GetModSystem().GetServerModId(formIdComponent.Id, modId, baseId))
        return;

    uint32_t cellBaseId = 0;
    uint32_t cellModId = 0;
    if (!m_world.GetModSystem().GetServerModId(pActor->parentCell->formID, cellModId, cellBaseId))
        return;

    AssignCharacterRequest message;

    message.Cookie = sCookieSeed;
    message.ReferenceId.BaseId = baseId;
    message.ReferenceId.ModId = modId;
    message.CellId.BaseId = cellBaseId;
    message.CellId.ModId = cellModId;

    if (const auto pWorldSpace = pActor->GetWorldSpace())
    {
        uint32_t worldSpaceBaseId = 0;
        uint32_t worldSpaceModId = 0;
        if (!m_world.GetModSystem().GetServerModId(pWorldSpace->formID, worldSpaceModId, worldSpaceBaseId))
            return;

        message.WorldSpaceId.BaseId = worldSpaceBaseId;
        message.WorldSpaceId.ModId = worldSpaceModId;
    }

    message.Position = pActor->position;
    message.Rotation.x = pActor->rotation.x;
    message.Rotation.y = pActor->rotation.z;

    // Serialize the base form
    const auto isPlayer = (formIdComponent.Id == 0x14);
    const auto isTemporary = pActor->formID >= 0xFF000000;

    if(isPlayer)
    {
        pNpc->MarkChanged(0x2000800);
    }

    const auto changeFlags = pNpc->GetChangeFlags();

    if(isPlayer || pNpc->formID >= 0xFF000000 || changeFlags != 0)
    {
        message.ChangeFlags = changeFlags;
        pNpc->Serialize(&message.AppearanceBuffer);
    }

#if TP_SKYRIM
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

            if(tints[i]->texture)
                entries[i].Name = tints[i]->texture->name.AsAscii();
        }
    }
#endif

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

    message.InventoryContent = pActor->GetInventory();
    message.FactionsContent = pActor->GetFactions();
    message.AllActorValues = pActor->GetEssentialActorValues();
    message.IsDead = pActor->IsDead();

    if(isTemporary)
    {
        if (!World::Get().GetModSystem().GetServerModId(pNpc->formID, message.FormId))
            return;
    }

    // Serialize actions
    auto* const pExtension = pActor->GetExtension();

    message.LatestAction = pExtension->LatestAnimation;
    pActor->SaveAnimationVariables(message.LatestAction.Variables);

    spdlog::info("Request id: {:X}, cookie: {:X}, {:X}", formIdComponent.Id, sCookieSeed, aEntity);

    if (m_transport.Send(message))
    {
        aRegistry.emplace<WaitingForAssignmentComponent>(aEntity, sCookieSeed);

        sCookieSeed++;
    }
}

void CharacterService::CancelServerAssignment(entt::registry& aRegistry, const entt::entity aEntity, const uint32_t aFormId) const noexcept
{
    if (aRegistry.all_of<RemoteComponent>(aEntity))
    {
        auto* const pForm = TESForm::GetById(aFormId);
        auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (pActor && ((pActor->formID & 0xFF000000) == 0xFF000000))
        {
            spdlog::info("Temporary Remote Deleted {:X}", aFormId);

            pActor->Delete();
        }

        aRegistry.remove<FaceGenComponent, InterpolationComponent, RemoteAnimationComponent,
                                   RemoteComponent, CacheComponent, WaitingFor3D>(aEntity);

        return;
    }

    // In the event we were waiting for assignment, drop it
    if (aRegistry.all_of<WaitingForAssignmentComponent>(aEntity))
    {
        auto& waitingComponent = aRegistry.get<WaitingForAssignmentComponent>(aEntity);

        CancelAssignmentRequest message;
        message.Cookie = waitingComponent.Cookie;

        m_transport.Send(message);

        aRegistry.remove<WaitingForAssignmentComponent>(aEntity);
    }

    if (aRegistry.all_of<LocalComponent>(aEntity))
    {
        auto& localComponent = aRegistry.get<LocalComponent>(aEntity);

        RequestOwnershipTransfer request;
        request.ServerId = localComponent.Id;

        m_transport.Send(request);

        aRegistry.remove<LocalAnimationComponent, LocalComponent>(aEntity);
    }
}

Actor* CharacterService::CreateCharacterForEntity(entt::entity aEntity) const noexcept
{
    auto* pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity);
    auto* pInterpolationComponent = m_world.try_get<InterpolationComponent>(aEntity);

    if (!pRemoteComponent || !pInterpolationComponent)
        return nullptr;

    auto& acMessage = pRemoteComponent->SpawnRequest;

    Actor* pActor = nullptr;

    // Custom forms
    if (acMessage.FormId == GameId{})
    {
        TESNPC* pNpc = nullptr;

        if (acMessage.BaseId != GameId{})
        {
            const auto cNpcId = World::Get().GetModSystem().GetGameId(acMessage.BaseId);
            if (cNpcId == 0)
            {
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod");
                return nullptr;
            }

            pNpc = RTTI_CAST(TESForm::GetById(cNpcId), TESForm, TESNPC);
            pNpc->Deserialize(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
        }
        else
        {
            pNpc = TESNPC::Create(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
            FaceGenSystem::Setup(m_world, aEntity, acMessage.FaceTints);
        }

        pActor = Actor::Create(pNpc);
    }

    if (!pActor)
        return nullptr;

    pActor->GetExtension()->SetRemote(true);
    pActor->rotation.x = acMessage.Rotation.x;
    pActor->rotation.z = acMessage.Rotation.y;
    pActor->MoveTo(PlayerCharacter::Get()->parentCell, pInterpolationComponent->Position);
    pActor->SetActorValues(acMessage.InitialActorValues);

    if (pActor->IsDead() != acMessage.IsDead)
        acMessage.IsDead ? pActor->Kill() : pActor->Respawn();

    m_world.emplace<WaitingFor3D>(aEntity);

    return pActor;
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

void CharacterService::RunRemoteUpdates() const noexcept
{
    // Delay by 120ms to let the interpolation system accumulate interpolation points
    const auto tick = m_transport.GetClock().GetCurrentTick() - 120;

    // Interpolation has to keep running even if the actor is not in view, otherwise we will never know if we need to spawn it
    auto interpolatedEntities =
        m_world.view<RemoteComponent, InterpolationComponent>();

    for (auto entity : interpolatedEntities)
    {
        auto* pFormIdComponent = m_world.try_get<FormIdComponent>(entity);
        auto& interpolationComponent = interpolatedEntities.get<InterpolationComponent>(entity);

        Actor* pActor = nullptr;
        if (pFormIdComponent)
        {
            auto* pForm = TESForm::GetById(pFormIdComponent->Id);
            pActor = RTTI_CAST(pForm, TESForm, Actor);
        }
       
        InterpolationSystem::Update(pActor, interpolationComponent, tick);
    }

    auto animatedView = m_world.view<RemoteComponent, RemoteAnimationComponent, FormIdComponent>();

    for (auto entity : animatedView)
    {
        auto& animationComponent = animatedView.get<RemoteAnimationComponent>(entity);
        auto& formIdComponent = animatedView.get<FormIdComponent>(entity);

        auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            continue;

        AnimationSystem::Update(m_world, pActor, animationComponent, tick);
    }

    auto facegenView = m_world.view<FormIdComponent, FaceGenComponent>();

    for(auto entity : facegenView)
    {
        auto& formIdComponent = facegenView.get<FormIdComponent>(entity);
        auto& faceGenComponent = facegenView.get<FaceGenComponent>(entity);

        const auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            continue;

        FaceGenSystem::Update(m_world, pActor, faceGenComponent);
    }

    auto waitingView = m_world.view<FormIdComponent, RemoteComponent, WaitingFor3D>();

    StackAllocator<1 << 13> allocator;
    ScopedAllocator _{allocator};

    Vector<entt::entity> toRemove;
    for (auto entity : waitingView)
    {
        auto& formIdComponent = waitingView.get<FormIdComponent>(entity);
        auto& remoteComponent = waitingView.get<RemoteComponent>(entity);

        const auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor || !pActor->GetNiNode())
            continue;

        pActor->SetInventory(remoteComponent.SpawnRequest.InventoryContent);
        pActor->SetFactions(remoteComponent.SpawnRequest.FactionsContent);
        pActor->LoadAnimationVariables(remoteComponent.SpawnRequest.LatestAction.Variables);

        if (pActor->IsDead() != remoteComponent.SpawnRequest.IsDead)
            remoteComponent.SpawnRequest.IsDead ? pActor->Kill() : pActor->Respawn();

        toRemove.push_back(entity);  
    }

    for (auto entity : toRemove)
        m_world.remove<WaitingFor3D>(entity);
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
        const auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
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

    if(!message.Changes.empty())
        m_transport.Send(message);
}

void CharacterService::RunSpawnUpdates() const noexcept
{
    auto invisibleView = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>(entt::exclude<FormIdComponent>);

    for (auto entity : invisibleView)
    {
        auto& remoteComponent = invisibleView.get<RemoteComponent>(entity);
        auto& interpolationComponent = invisibleView.get<InterpolationComponent>(entity);

        if (const auto pWorldSpace = PlayerCharacter::Get()->GetWorldSpace())
        {
            float characterX = interpolationComponent.Position.x;
            float characterY = interpolationComponent.Position.y;
            const auto characterCoords = GridCellCoords::CalculateGridCellCoords(characterX, characterY);
            const auto* pTES = TES::Get();
            const auto playerCoords = GridCellCoords::GridCellCoords(pTES->centerGridX, pTES->centerGridY);

            if (GridCellCoords::IsCellInGridCell(characterCoords, playerCoords))
            {
                auto* pActor = RTTI_CAST(TESForm::GetById(remoteComponent.CachedRefId), TESForm, Actor);
                if (!pActor)
                {
                    pActor = CreateCharacterForEntity(entity);
                    if (!pActor)
                    {
                        continue;
                    }

                    remoteComponent.CachedRefId = pActor->formID;
                }

                pActor->MoveTo(PlayerCharacter::Get()->parentCell, interpolationComponent.Position);
            }
        }
    }
}

void CharacterService::OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept
{
#if TP_SKYRIM64
    TP_ASSERT(acSpellCastEvent.pSpell, "SpellCastEvent has no spell");

    if (!acSpellCastEvent.pCaster->pCasterActor || !acSpellCastEvent.pCaster->pCasterActor->GetNiNode())
    {
        spdlog::warn("Spell cast event has no actor or actor is not loaded");
        return;
    }

    uint32_t formId = acSpellCastEvent.pCaster->pCasterActor->formID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (casterEntityIt == std::end(view))
        return;

    auto& localComponent = view.get<LocalComponent>(*casterEntityIt);

    SpellCastRequest request;
    request.CasterId = localComponent.Id;
    request.CastingSource = acSpellCastEvent.pCaster->GetCastingSource();
    request.IsDualCasting = acSpellCastEvent.pCaster->GetIsDualCasting();
    m_world.GetModSystem().GetServerModId(acSpellCastEvent.pSpell->formID, request.SpellFormId.ModId,
                                          request.SpellFormId.BaseId);

    spdlog::info("Spell cast event sent, ID: {:X}, Source: {}, IsDualCasting: {}", request.CasterId,
                 request.CastingSource, request.IsDualCasting);

    m_transport.Send(request);
#endif
}

void CharacterService::OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept
{
#if TP_SKYRIM64
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Caster with remote id {:X} not found.", acMessage.CasterId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

    auto* pForm = TESForm::GetById(formIdComponent.Id);
    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

    if (!pActor->leftHandCaster)
        pActor->leftHandCaster = (ActorMagicCaster*)pActor->GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
    if (!pActor->rightHandCaster)
        pActor->rightHandCaster = (ActorMagicCaster*)pActor->GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
    if (!pActor->shoutCaster)
        pActor->shoutCaster = (ActorMagicCaster*)pActor->GetMagicCaster(MagicSystem::CastingSource::OTHER);

    // Only left hand casters need dual casting (?)
    pActor->leftHandCaster->SetDualCasting(acMessage.IsDualCasting);

    MagicItem* pSpell = nullptr;

    if (acMessage.CastingSource >= 4)
    {
        spdlog::warn("Casting source out of bounds, trying form id");
    }
    else
    {
        pSpell = pActor->magicItems[acMessage.CastingSource];
    }

    if (!pSpell)
    {
        const uint32_t cSpellFormId = World::Get().GetModSystem().GetGameId(acMessage.SpellFormId);
        if (cSpellFormId == 0)
        {
            spdlog::error("Could not find spell form id for GameId base {:X}, mod {:X}", acMessage.SpellFormId.BaseId,
                          acMessage.SpellFormId.ModId);
            return;
        }
        auto* pSpellForm = TESForm::GetById(cSpellFormId);
        if (!pSpellForm)
        {
            spdlog::error("Cannot find spell form");
        }
        else
            pSpell = RTTI_CAST(pSpellForm, TESForm, MagicItem);
    }

    switch (acMessage.CastingSource)
    {
    case MagicSystem::CastingSource::LEFT_HAND:
        pActor->leftHandCaster->CastSpellImmediate(pSpell, false, nullptr, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::RIGHT_HAND:
        pActor->rightHandCaster->CastSpellImmediate(pSpell, false, nullptr, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::OTHER:
        pActor->shoutCaster->CastSpellImmediate(pSpell, false, nullptr, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::INSTANT:
        break;
    }
#endif
}


void CharacterService::OnProjectileLaunchedEvent(const ProjectileLaunchedEvent& acEvent) const noexcept
{
#if 1
    auto& modSystem = m_world.Get().GetModSystem();


    auto shooterFormId = acEvent.ShooterID;
    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto shooterEntityIt = std::find_if(std::begin(view), std::end(view), [shooterFormId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == shooterFormId;
    });

    if (shooterEntityIt == std::end(view))
        return;

    auto& localComponent = view.get<LocalComponent>(*shooterEntityIt);

    ProjectileLaunchRequest request{};

    request.ShooterID = localComponent.Id;

    request.OriginX = acEvent.Origin.x;
    request.OriginY = acEvent.Origin.y;
    request.OriginZ = acEvent.Origin.z;

    modSystem.GetServerModId(acEvent.ProjectileBaseID, request.ProjectileBaseID);
    modSystem.GetServerModId(acEvent.WeaponID, request.WeaponID);
    modSystem.GetServerModId(acEvent.AmmoID, request.AmmoID);

    request.ZAngle = acEvent.ZAngle;
    request.XAngle = acEvent.XAngle;
    request.YAngle = acEvent.YAngle;

    modSystem.GetServerModId(acEvent.ParentCellID, request.ParentCellID);
    modSystem.GetServerModId(acEvent.SpellID, request.SpellID);

    request.CastingSource = acEvent.CastingSource;

#if TP_SKYRIM64
    request.unkBool1 = acEvent.unkBool1;
#else
#endif

    request.Area = acEvent.Area;
    request.Power = acEvent.Power;
    request.Scale = acEvent.Scale;

    request.AlwaysHit = acEvent.AlwaysHit;
    request.NoDamageOutsideCombat = acEvent.NoDamageOutsideCombat;
    request.AutoAim = acEvent.AutoAim;
    request.UseOrigin = acEvent.UseOrigin;
    request.DeferInitialization = acEvent.DeferInitialization;
    request.Tracer = acEvent.Tracer;
    request.ForceConeOfFire = acEvent.ForceConeOfFire;

    spdlog::info("Ammo event id: {}, request id: {}", acEvent.AmmoID, request.AmmoID.BaseId);

    m_transport.Send(request);
#endif
}

void CharacterService::OnNotifyProjectileLaunch(const NotifyProjectileLaunch& acMessage) const noexcept
{
#if 1
    auto& modSystem = World::Get().GetModSystem();

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ShooterID](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Shooter with remote id {:X} not found.", acMessage.ShooterID);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

#if TP_SKYRIM64
    Projectile::LaunchData launchData{};
#else
    ProjectileLaunchData launchData{};
#endif

#if TP_FALLOUT4
    launchData.pShooter = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, TESObjectREFR);
#endif

    launchData.Origin.x = acMessage.OriginX;
    launchData.Origin.y = acMessage.OriginY;
    launchData.Origin.z = acMessage.OriginZ;

    const auto cProjectileBaseId = modSystem.GetGameId(acMessage.ProjectileBaseID);
    launchData.pProjectileBase = TESForm::GetById(cProjectileBaseId);

    //const auto cFromWeaponId = modSystem.GetGameId(acMessage.WeaponID);
#if TP_SKYRIM64
    //launchData.pFromWeapon = RTTI_CAST(TESForm::GetById(cFromWeaponId), TESForm, TESObjectWEAP);
#else
    Actor* pShooter = RTTI_CAST(launchData.pShooter, TESObjectREFR, Actor);
    pShooter->GetCurrentWeapon(&launchData.FromWeapon, 0);
#endif

    const auto cFromAmmoId = modSystem.GetGameId(acMessage.AmmoID);
    launchData.pFromAmmo = RTTI_CAST(TESForm::GetById(cFromAmmoId), TESForm, TESAmmo);

    launchData.fZAngle = acMessage.ZAngle;
    launchData.fXAngle = acMessage.XAngle;
    launchData.fYAngle = acMessage.YAngle;

    //const auto cParentCellId = modSystem.GetGameId(acMessage.ParentCellID);
    launchData.pParentCell = PlayerCharacter::Get()->parentCell;

    const auto cSpellId = modSystem.GetGameId(acMessage.SpellID);
    launchData.pSpell = RTTI_CAST(TESForm::GetById(cSpellId), TESForm, MagicItem);

    launchData.eCastingSource = (MagicSystem::CastingSource)acMessage.CastingSource;

#if TP_SKYRIM64
    launchData.unkBool1 = acMessage.unkBool1;
#else
#endif

    launchData.iArea = acMessage.Area;
    launchData.fPower = acMessage.Power;
    launchData.fScale = acMessage.Scale;

    launchData.bAlwaysHit = acMessage.AlwaysHit;
    launchData.bNoDamageOutsideCombat = acMessage.NoDamageOutsideCombat;
    launchData.bAutoAim = acMessage.AutoAim;
    launchData.bUseOrigin = acMessage.UseOrigin;
    launchData.bDeferInitialization = acMessage.DeferInitialization;
    launchData.bTracer = acMessage.Tracer;
    launchData.bForceConeOfFire = acMessage.ForceConeOfFire;

#if TP_FALLOUT4
    launchData.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;
    launchData.fConeOfFireRadiusMult = 1.0f;
    launchData.eTargetLimb = -1;
    launchData.bAllow3D = true;
#endif

    uint8_t resultBuffer[100];

    Projectile::Launch(resultBuffer, launchData);
#endif
}

void CharacterService::OnInterruptCast(const InterruptCastEvent& acEvent) const noexcept
{
#if TP_SKYRIM64
    auto formId = acEvent.CasterFormID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (casterEntityIt == std::end(view))
        return;

    auto& localComponent = view.get<LocalComponent>(*casterEntityIt);

    InterruptCastRequest request;
    request.CasterId = localComponent.Id;
    m_transport.Send(request);
#endif
}

void CharacterService::OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept
{
#if TP_SKYRIM64
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Caster with remote id {:X} not found.", acMessage.CasterId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

    auto* pForm = TESForm::GetById(formIdComponent.Id);
    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

    pActor->InterruptCast(false);

    spdlog::info("Interrupt remote cast successful");
#endif
}

void CharacterService::OnAddTargetEvent(const AddTargetEvent& acEvent) const noexcept
{
#if TP_SKYRIM64
    auto view = m_world.view<FormIdComponent>();
    for (auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        if (formIdComponent.Id != acEvent.TargetID)
            continue;

        AddTargetRequest request;

        if (const auto* pLocalComponent = m_world.try_get<LocalComponent>(entity))
        {
            request.TargetId = pLocalComponent->Id;
        }
        else if (const auto* pRemoteComponent = m_world.try_get<RemoteComponent>(entity))
        {
            request.TargetId = pRemoteComponent->Id;
        }

        TP_ASSERT(request.TargetId, "AddTargetRequest must have a target id.");

        if (!m_world.GetModSystem().GetServerModId(acEvent.SpellID, request.SpellId.ModId, request.SpellId.BaseId))
        {
            spdlog::error("{s}: Could not find spell with from {:X}", __FUNCTION__, acEvent.SpellID);
            return;
        }

        m_transport.Send(request);

        break;
    }
#endif
}

void CharacterService::OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept
{
#if TP_SKYRIM64
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        uint32_t componentId;
        const auto cpLocalComponent = m_world.try_get<LocalComponent>(entity);
        const auto cpRemoteComponent = m_world.try_get<RemoteComponent>(entity);

        if (cpLocalComponent)
            componentId = cpLocalComponent->Id;
        else if (cpRemoteComponent)
            componentId = cpRemoteComponent->Id;
        else
            continue;

        if (componentId == acMessage.TargetId)
        {
            auto& formIdComponent = view.get<FormIdComponent>(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            TP_ASSERT(pActor, "Actor should exist, form id: {:X}", formIdComponent.Id);

            if (pActor)
            {
                const auto cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
                if (cSpellId == 0)
                {
                    spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__,
                                  acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
                    return;
                }

                auto* pSpell = static_cast<MagicItem*>(TESForm::GetById(cSpellId));
                if (!pSpell)
                {
                    spdlog::error("{}: Failed to retrieve spell by id {:X}", cSpellId);
                    return;
                }

                // TODO: AddTarget gets notified for every effect, but we loop through the effects here again
                for (auto effect : pSpell->listOfEffects)
                {
                    MagicTarget::AddTargetData data{};
                    data.pSpell = pSpell;
                    data.pEffectItem = effect;
                    data.fMagnitude = 0.0f;
                    data.fUnkFloat1 = 1.0f;
                    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;

                    pActor->magicTarget.AddTarget(data);
                }

                break;
            }
        }
    }
#endif
}
