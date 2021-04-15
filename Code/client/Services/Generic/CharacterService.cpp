#include <TiltedOnlinePCH.h>

#include "Forms/TESObjectCELL.h"
#include "Forms/TESWorldSpace.h"
#include "Games/Misc/UI.h"
#include "Messages/CharacterTravelRequest.h"
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

#include <Structs/ActionEvent.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/RemoveCharacterRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/NotifyCharacterTravel.h>
#include <Messages/RequestSpawnData.h>
#include <Messages/NotifySpawnData.h>

#include <World.h>


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
    m_equipmentConnection = m_dispatcher.sink<EquipmentChangeEvent>().connect<&CharacterService::OnEquipmentChangeEvent>(this);
    m_inventoryConnection = m_dispatcher.sink<NotifyInventoryChanges>().connect<&CharacterService::OnInventoryChanges>(this);
    m_factionsConnection = m_dispatcher.sink<NotifyFactionsChanges>().connect<&CharacterService::OnFactionsChanges>(this);
    m_removeCharacterConnection = m_dispatcher.sink<NotifyRemoveCharacter>().connect<&CharacterService::OnRemoveCharacter>(this);
    m_characterTravelConnection = m_dispatcher.sink<NotifyCharacterTravel>().connect<&CharacterService::OnCharacterTravel>(this);
    m_remoteSpawnDataReceivedConnection = m_dispatcher.sink<NotifySpawnData>().connect<&CharacterService::OnRemoteSpawnDataReceived>(this);
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
    RunInventoryUpdates();
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

        pActor->SetActorValues(acMessage.AllActorValues);
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
    }
}

void CharacterService::OnCharacterSpawn(const CharacterSpawnRequest& acMessage) const noexcept
{
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
        return;

    auto& remoteComponent = m_world.emplace_or_replace<RemoteComponent>(*entity, acMessage.ServerId, pActor->formID);
    remoteComponent.SpawnRequest = acMessage;

    auto& interpolationComponent = InterpolationSystem::Setup(m_world, *entity);
    interpolationComponent.Position = acMessage.Position;

    AnimationSystem::Setup(m_world, *entity);

    pActor->GetExtension()->SetRemote(true);
    pActor->rotation.x = acMessage.Rotation.x;
    pActor->rotation.z = acMessage.Rotation.y;
    pActor->MoveTo(PlayerCharacter::Get()->parentCell, acMessage.Position);

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

        auto& formIdComponent = view.get<FormIdComponent>(*itor);
        auto* const pForm = TESForm::GetById(formIdComponent.Id);
        auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
            return;

        pActor->SetActorValues(remoteComponent.SpawnRequest.InitialActorValues);
        pActor->SetInventory(remoteComponent.SpawnRequest.InventoryContent);
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

void CharacterService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
    m_charactersWithInventoryChanges.insert(acEvent.ActorId);
}

void CharacterService::OnInventoryChanges(const NotifyInventoryChanges& acEvent) noexcept
{
    for (const auto& [id, inventory] : acEvent.Changes)
    {
        m_cachedInventoryChanges[id] = inventory;
    }

    ApplyCachedInventoryChanges();
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

void CharacterService::OnRemoveCharacter(const NotifyRemoveCharacter& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acEvent.ServerId, view](entt::entity entity) {
            return view.get<RemoteComponent>(entity).Id == id;
        });

    if (itor != std::end(view))
    {
        if (auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*itor))
        {
            spdlog::info("\tformid: {:X}", pFormIdComponent->Id);
            
            const auto pActor = RTTI_CAST(TESForm::GetById(pFormIdComponent->Id), TESForm, Actor);
            if (!pActor)
                return;

            pActor->Delete();
        }

        m_world.remove_if_exists<RemoteComponent, RemoteAnimationComponent, InterpolationComponent>(*itor);
    }
}

void CharacterService::OnCharacterTravel(const NotifyCharacterTravel& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [&acEvent, &view](auto entity) {
        return view.get<RemoteComponent>(entity).Id == acEvent.ServerId;
    });

    if (itor != std::end(view))
    {
        auto& formIdComponent = view.get<FormIdComponent>(*itor);

        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (pActor)
        {
            pActor->GetExtension()->SetRemote(false);

            const auto cCellId = World::Get().GetModSystem().GetGameId(acEvent.CellId);
            if (cCellId != 0)
            {
                auto* const pCell = RTTI_CAST(TESForm::GetById(cCellId), TESForm, TESObjectCELL);
                if (pCell)
                {
                    pActor->MoveTo(pCell, acEvent.Position);
                }
            }
        }

        m_world.remove_if_exists<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>(*itor);
    }
}

void CharacterService::RequestServerAssignment(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    static uint32_t sCookieSeed = 0;

    const auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
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
    if (!m_world.GetModSystem().GetServerModId(pActor->GetCellId(), cellModId, cellBaseId))
        return;

    AssignCharacterRequest message;

    message.Cookie = sCookieSeed;
    message.ReferenceId.BaseId = baseId;
    message.ReferenceId.ModId = modId;
    message.CellId.BaseId = cellBaseId;
    message.CellId.ModId = cellModId;

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
        // Do nothing, a system will detect a remote component missing a form id component later on and try to spawn it
        auto* const pForm = TESForm::GetById(aFormId);
        auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (pActor && ((pActor->formID & 0xFF000000) == 0xFF000000))
        {
            const auto pWorldSpace = pActor->GetWorldSpace();
            const auto pPlayerWorldSpace = PlayerCharacter::Get()->GetWorldSpace();

            if (pWorldSpace != pPlayerWorldSpace)
            {
                spdlog::info("Remote Deleted {:X}", aFormId);

                pActor->Delete();

                aRegistry.remove_if_exists<FaceGenComponent, InterpolationComponent, RemoteAnimationComponent,
                                           RemoteComponent, CacheComponent, WaitingFor3D>(aEntity);
            }
        }

        return;
    }

    // In the event we were waiting for assignment, drop it
    if (aRegistry.all_of<WaitingForAssignmentComponent>(aEntity))
    {
        auto& waitingComponent = aRegistry.get<WaitingForAssignmentComponent>(aEntity);

        CancelAssignmentRequest message;
        message.Cookie = waitingComponent.Cookie;

        m_transport.Send(message);

        aRegistry.remove_if_exists<WaitingForAssignmentComponent>(aEntity);
    }

    if (aRegistry.all_of<LocalComponent>(aEntity))
    {
        auto& localComponent = aRegistry.get<LocalComponent>(aEntity);

        auto* const pForm = TESForm::GetById(aFormId);
        auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);

        bool actorWasMoved = false;

        if (pActor)
        {
            const auto pWorldSpace = pActor->GetWorldSpace();
            const auto pPlayerWorldSpace = PlayerCharacter::Get()->GetWorldSpace();

            if (pWorldSpace != pPlayerWorldSpace && pActor->parentCell)
            {
                actorWasMoved = true;

                CharacterTravelRequest message;
                message.ServerId = localComponent.Id;
                message.Position = pActor->position;
                m_world.GetModSystem().GetServerModId(pActor->parentCell->formID, message.CellId);

                m_transport.Send(message);
            }
        }
        
        if (!actorWasMoved)
        {
            RemoveCharacterRequest message;
            message.ServerId = localComponent.Id;

            m_transport.Send(message);
        }

        aRegistry.remove_if_exists<LocalAnimationComponent, LocalComponent>(aEntity);
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

        toRemove.push_back(entity);  
    }

    for (auto entity : toRemove)
        m_world.remove<WaitingFor3D>(entity);
}

void CharacterService::RunInventoryUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_charactersWithInventoryChanges.empty())
    {
        RequestInventoryChanges message;

        auto animatedLocalView = m_world.view<LocalComponent, LocalAnimationComponent, FormIdComponent>();
        for (auto entity : animatedLocalView)
        {
            auto& formIdComponent = animatedLocalView.get<FormIdComponent>(entity);
            auto& localComponent = animatedLocalView.get<LocalComponent>(entity);

            if (m_charactersWithInventoryChanges.find(formIdComponent.Id) == std::end(m_charactersWithInventoryChanges))
                continue;

            const auto* pForm = TESForm::GetById(formIdComponent.Id);
            const auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                continue;

            message.Changes[localComponent.Id] = pActor->GetInventory();
        }

        m_transport.Send(message);

        m_charactersWithInventoryChanges.clear();
    }

    ApplyCachedInventoryChanges();
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

        if (distance2(PlayerCharacter::Get()->position, interpolationComponent.Position) < (12000.f * 12000.f))
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

        /*spdlog::info("Remote entity {:X} with cached id {:X} is invisible", remoteComponent.Id,
                     remoteComponent.CachedRefId);*/
    }
}

void CharacterService::ApplyCachedInventoryChanges() noexcept
{
    if (UI::Get()->IsOpen(BSFixedString("ContainerMenu")))
        return;

    auto view = m_world.view<RemoteComponent, FormIdComponent>();
    for (const auto& [id, inventory] : m_cachedInventoryChanges)
    {
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = id, view](entt::entity entity) {
            return view.get<RemoteComponent>(entity).Id == id;
        });

        if (itor != std::end(view))
        {
            auto& formIdComponent = view.get<FormIdComponent>(*itor);
            auto& remoteComponent = view.get<RemoteComponent>(*itor);

            auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
            if (!pActor)
                return;

            remoteComponent.SpawnRequest.InventoryContent = inventory;
            pActor->SetInventory(inventory);
        }
    }
    m_cachedInventoryChanges.clear();
}
