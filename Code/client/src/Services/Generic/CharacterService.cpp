#include <Services/CharacterService.h>
#include <Services/TransportService.h>
#include <Services/QuestService.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/TESNPC.h>
#include <Games/Skyrim/Forms/TESQuest.h>
#include <Games/Skyrim/Misc/ActorProcessManager.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>
#include <Games/Skyrim/ExtraData/ExtraLeveledCreature.h>

#include <Games/Fallout4/Forms/TESNPC.h>
#include <Games/Fallout4/Forms/TESQuest.h>
#include <Games/Fallout4/Misc/ProcessManager.h>
#include <Games/Fallout4/Misc/MiddleProcess.h>
#include <Games/Fallout4/ExtraData/ExtraLeveledCreature.h>

#include <Components.h>

#include <Systems/InterpolationSystem.h>
#include <Systems/AnimationSystem.h>
#include <Systems/FaceGenSystem.h>
#include <Systems/CacheSystem.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ReferenceSpawnedEvent.h>
#include <Events/EquipmentChangeEvent.h>

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

#include <World.h>


CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_formIdAddedConnection = m_world.on_construct<FormIdComponent>().connect<&CharacterService::OnFormIdComponentAdded>(this);
    m_formIdRemovedConnection = m_world.on_destroy<FormIdComponent>().connect<&CharacterService::OnFormIdComponentRemoved>(this);

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
}

void CharacterService::OnFormIdComponentAdded(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    spdlog::info("FormId added {:X}", formIdComponent.Id);

    if (aRegistry.has<RemoteComponent>(aEntity) || aRegistry.has<LocalComponent>(aEntity) || aRegistry.has<WaitingForAssignmentComponent>(aEntity))
        return;

    // Security check
    auto* const pForm = TESForm::GetById(formIdComponent.Id);
    auto* const pActor = RTTI_CAST(pForm, TESForm, Actor);
    if(!pActor)
        return;

    CacheSystem::Setup(World::Get(), aEntity, pActor);

    auto* const pNpc = RTTI_CAST(pActor->baseForm, TESForm, TESNPC);

    auto* pExtra = static_cast<ExtraLeveledCreature*>(pActor->GetExtraDataList()->GetByType(ExtraData::LeveledCreature));

    auto* const pOwner = pNpc->actorData.owner;
    if (pOwner)
    {
        spdlog::info("\tOwner: type {}, id {:X}", static_cast<uint32_t>(pOwner->formType), pOwner->formID);
    }

    if(pNpc)
    {
        RequestServerAssignment(aRegistry, aEntity);
    }
}

void CharacterService::OnFormIdComponentRemoved(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    spdlog::info("FormId removed {:X}", formIdComponent.Id);

    CancelServerAssignment(aRegistry, aEntity);
}

void CharacterService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunLocalUpdates();
    RunInventoryUpdates();
    RunFactionsUpdates();
    RunRemoteUpdates();
}

void CharacterService::OnConnected(const ConnectedEvent& acConnectedEvent) const noexcept
{
    // Go through all the forms that were previously detected
    auto view = m_world.view<FormIdComponent>();
    for (auto entity : view)
    {
        OnFormIdComponentAdded(m_world, entity);
    }
}

void CharacterService::OnDisconnected(const DisconnectedEvent& acDisconnectedEvent) const noexcept
{
    m_world.clear<WaitingForAssignmentComponent, LocalComponent, RemoteComponent>();
}

void CharacterService::OnAssignCharacter(const AssignCharacterResponse& acMessage) const noexcept
{
    auto view = m_world.view<WaitingForAssignmentComponent>();
    const auto itor = std::find_if(std::begin(view), std::end(view), [view, cookie = acMessage.Cookie](auto entity)
    {
        return view.get(entity).Cookie == cookie;
    });

    if (itor == std::end(view))
        return;

    if (acMessage.Owner)
    {
        m_world.emplace<LocalComponent>(*itor, acMessage.ServerId);
        m_world.emplace<LocalAnimationComponent>(*itor);
    }
    else
    {
        m_world.emplace<RemoteComponent>(*itor, acMessage.ServerId);

        const auto& formIdComponent = m_world.get<FormIdComponent>(*itor);

        auto* const pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            return;

        pActor->GetExtension()->SetRemote(true);

        InterpolationSystem::Setup(m_world, *itor);
        AnimationSystem::Setup(m_world, *itor);
    }
    
    m_world.remove<WaitingForAssignmentComponent>(*itor);
}

void CharacterService::OnCharacterSpawn(const CharacterSpawnRequest& acMessage) const noexcept
{
    Actor* pActor = nullptr;

    const auto cEntity = m_world.create();

    // Custom forms
    if (acMessage.FormId == GameId{})
    {
        TESNPC* pNpc = nullptr;

        if (acMessage.BaseId != GameId{})
        {
            const auto cNpcId = World::Get().GetModSystem().GetGameId(acMessage.BaseId);
            if(cNpcId == 0)
            {
                m_world.destroy(cEntity);
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod");
                return;
            }

            pNpc = RTTI_CAST(TESForm::GetById(cNpcId), TESForm, TESNPC);
            pNpc->Deserialize(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
        }
        else
        {
            pNpc = TESNPC::Create(acMessage.AppearanceBuffer, acMessage.ChangeFlags);
            FaceGenSystem::Setup(m_world, cEntity, acMessage.FaceTints);
        }

        pActor = Actor::Create(RTTI_CAST(pNpc, TESForm, TESNPC));
    }
    else
    {
        const auto cActorId = World::Get().GetModSystem().GetGameId(acMessage.FormId);
        auto* const pForm = TESForm::GetById(cActorId);

        pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
        {
            m_world.destroy(cEntity);
            spdlog::error("Failed to retrieve Actor {:X}, it will not be spawned, possibly missing mod", cActorId);
            spdlog::error("\tForm : {:X}", pForm ? pForm->formID : 0);
            return;
        }
    }

    if (!pActor)
        return;

    m_world.emplace<RemoteComponent>(cEntity, acMessage.ServerId);

    InterpolationSystem::Setup(m_world, cEntity);
    AnimationSystem::Setup(m_world, cEntity);

    pActor->GetExtension()->SetRemote(true);
    pActor->SetInventory(acMessage.InventoryContent);
    pActor->SetFactions(acMessage.FactionsContent);
    pActor->Enable();
    //pActor->MoveTo(PlayerCharacter::Get(), Vector3<float>{}, true);

    auto& remoteAnimationComponent = m_world.get<RemoteAnimationComponent>(cEntity);
    remoteAnimationComponent.TimePoints.push_back(acMessage.LatestAction);

    m_dispatcher.trigger(ReferenceSpawnedEvent(pActor->formID, pActor->formType, cEntity));
}

void CharacterService::OnReferencesMoveRequest(const ServerReferencesMoveRequest& acMessage) const noexcept
{
    auto view = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>();

    for (const auto& entry : acMessage.Updates)
    {
        auto itor = std::find_if(std::begin(view), std::end(view), [serverId = entry.first, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == serverId;
        });

        if (itor == std::end(view))
            continue;

        auto& interpolationComponent = view.get<InterpolationComponent>(*itor);
        auto& animationComponent = view.get<RemoteAnimationComponent>(*itor);
        const auto& update = entry.second;
        const auto& movement = update.UpdatedMovement;

        InterpolationComponent::TimePoint point;
        point.Tick = acMessage.Tick;
        point.Position = movement.Position;
        point.Rotation = Vector3<float>(movement.Rotation.X, 0.f, movement.Rotation.Y);
        point.Variables = movement.Variables;
        point.Direction = movement.Direction;

        InterpolationSystem::AddPoint(interpolationComponent, point);

        for (auto& action : update.ActionEvents)
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

void CharacterService::OnInventoryChanges(const NotifyInventoryChanges& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent>();

    for (const auto& change : acEvent.Changes)
    {
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = change.first, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == id;
        });

        if (itor != std::end(view))
        {
            auto& formIdComponent = view.get<FormIdComponent>(*itor);

            auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
            if (!pActor)
                return;

            pActor->SetInventory(change.second);
        }
    }
}

void CharacterService::OnFactionsChanges(const NotifyFactionsChanges& acEvent) const noexcept
{
    auto view = m_world.view<RemoteComponent, FormIdComponent, CacheComponent>();

    for (const auto& change : acEvent.Changes)
    {
        const auto itor = std::find_if(std::begin(view), std::end(view), [id = change.first, view](entt::entity entity)
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
            cacheComponent.FactionsContent = std::move(change.second);

            pActor->SetFactions(cacheComponent.FactionsContent);
        }
    }
}

void CharacterService::OnRemoveCharacter(const NotifyRemoveCharacter& acEvent) const noexcept
{
    /*auto view = m_world.view<RemoteComponent, FormIdComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acEvent.ServerId, view](entt::entity entity) {
            return view.get<RemoteComponent>(entity).Id == id;
        });

    spdlog::info("RemoveCharacter {:X}", acEvent.ServerId);

    if (itor != std::end(view))
    {
        auto& formIdComponent = view.get<FormIdComponent>(*itor);

        spdlog::info("\tformid: {:X}", formIdComponent.Id);

        const auto pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (!pActor || !pActor->GetNiNode())
            return;

        pActor->Disable();

        InterpolationSystem::Clean(m_world, *itor);
        AnimationSystem::Clean(m_world, *itor);
    }*/
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

    message.Position.m_x = pActor->position.x;
    message.Position.m_y = pActor->position.y;
    message.Position.m_z = pActor->position.z;

    message.Rotation.X = pActor->rotation.x;
    message.Rotation.Y = pActor->rotation.z;

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

        auto& tints = PlayerCharacter::Get()->GetTints();

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
                GameId Id{};

                if (modSystem.GetServerModId(pQuest->formID, Id))
                {
                    auto& entry = questLog.emplace_back();
                    entry.Stage = pQuest->currentStage;
                    entry.Id = Id;
                }
            }
        }

        // remove duplicates
        const auto ip = std::unique(questLog.begin(), questLog.end());
        questLog.resize(std::distance(questLog.begin(), ip));
    }

    message.InventoryContent = pActor->GetInventory();
    message.FactionsContent = pActor->GetFactions();

    if(isTemporary)
    {
        if (!World::Get().GetModSystem().GetServerModId(pNpc->formID, message.FormId))
            return;
    }

    // Serialize actions
    auto* const pExtension = pActor->GetExtension();

    message.LatestAction = pExtension->LatestAnimation;

    if (m_transport.Send(message))
    {
        aRegistry.emplace<WaitingForAssignmentComponent>(aEntity, sCookieSeed);

        ++sCookieSeed;
    }
}

void CharacterService::CancelServerAssignment(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    // In the event we were waiting for assignment, drop it
    if (aRegistry.has<WaitingForAssignmentComponent>(aEntity))
    {
        auto& waitingComponent = aRegistry.get<WaitingForAssignmentComponent>(aEntity);

        CancelAssignmentRequest message;
        message.Cookie = waitingComponent.Cookie;

        m_transport.Send(message);

        aRegistry.remove<WaitingForAssignmentComponent>(aEntity);
    }

    if (aRegistry.has<LocalComponent>(aEntity))
    {
        auto& localComponent = aRegistry.get<LocalComponent>(aEntity);

        RemoveCharacterRequest message;
        message.ServerId = localComponent.Id;

        m_transport.Send(message);

        aRegistry.remove<LocalComponent>(aEntity);
    }

    if (aRegistry.has<RemoteComponent>(aEntity))
    {
        // Do nothing, a system will detect a remote component missing a form id component later on and try to spawn it
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

void CharacterService::RunRemoteUpdates() const noexcept
{
    // Delay by 120ms to let the interpolation system accumulate interpolation points
    auto tick = m_transport.GetClock().GetCurrentTick() - 120;

    auto animatedView = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, FormIdComponent>();

    for(auto entity : animatedView)
    {
        auto& formIdComponent = animatedView.get<FormIdComponent>(entity);
        auto& interpolationComponent = animatedView.get<InterpolationComponent>(entity);
        auto& animationComponent = animatedView.get<RemoteAnimationComponent>(entity);

        const auto pForm = TESForm::GetById(formIdComponent.Id);
        const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            continue;

        InterpolationSystem::Update(pActor, interpolationComponent, tick);
        AnimationSystem::Update(m_world, pActor, animationComponent, tick);
    }

    auto facegenView = m_world.view<FormIdComponent, FaceGenComponent>();

    for(auto entity : facegenView)
    {
        auto& formIdComponent = facegenView.get<FormIdComponent>(entity);
        auto& faceGenComponent = facegenView.get<FaceGenComponent>(entity);

        const auto pForm = TESForm::GetById(formIdComponent.Id);
        const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            continue;

        FaceGenSystem::Update(m_world, pActor, faceGenComponent);
    }

    m_world.group<RemoteComponent>(entt::exclude<FormIdComponent>).each([](auto entity, auto& remoteComponent)
        {
            // The entity has a remote component but no form id so we need to spawn it
            TP_UNUSED(entity);
            TP_UNUSED(remoteComponent);
        });
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

            const auto pForm = TESForm::GetById(formIdComponent.Id);
            const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                continue;

            message.Changes[localComponent.Id] = pActor->GetInventory();
        }

        m_transport.Send(message);

        m_charactersWithInventoryChanges.clear();
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

        const auto pForm = TESForm::GetById(formIdComponent.Id);
        const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            continue;

        // Check if cached factions and current factions are identical
        auto factions = pActor->GetFactions();

        if (cacheComponent.FactionsContent == factions)
            continue;

        cacheComponent.FactionsContent = factions;

        // If not send the current factions and replace the cached factions
        message.Changes[localComponent.Id] = std::move(factions);
    }

    if(!message.Changes.empty())
        m_transport.Send(message);
}
