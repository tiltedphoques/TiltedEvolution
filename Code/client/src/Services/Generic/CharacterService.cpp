#include <Services/CharacterService.h>
#include <Services/TransportService.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/TESNPC.h>
#include <Games/Skyrim/Misc/ActorProcessManager.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>

#include <Games/Fallout4/Forms/TESNPC.h>
#include <Games/Fallout4/Misc/ProcessManager.h>
#include <Games/Fallout4/Misc/MiddleProcess.h>

#include <Components.h>

#include <Systems/InterpolationSystem.h>
#include <Systems/AnimationSystem.h>
#include <Systems/FaceGenSystem.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ReferenceSpawnedEvent.h>
#include <Events/ActionEvent.h>
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

    m_characterAssignConnection = m_dispatcher.sink<TiltedMessages::CharacterAssignResponse>().connect<&CharacterService::OnCharacterAssign>(this);
    m_characterSpawnConnection = m_dispatcher.sink<TiltedMessages::CharacterSpawnRequest>().connect<&CharacterService::OnCharacterSpawn>(this);
    m_referenceMovementSnapshotConnection = m_dispatcher.sink<TiltedMessages::ReferenceMovementSnapshot>().connect<&CharacterService::OnReferenceMovementSnapshot>(this);
}

void CharacterService::OnFormIdComponentAdded(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    spdlog::info("FormId added {:X}", formIdComponent.Id);

    if (aRegistry.has<RemoteComponent>(aEntity) || aRegistry.has<LocalComponent>(aEntity) || aRegistry.has<WaitingForAssignmentComponent>(aEntity))
        return;

    // Security check
    const auto pForm = TESForm::GetById(formIdComponent.Id);
    const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
    if(pActor == nullptr)
        return;

    const auto pNpc = RTTI_CAST(pActor->baseForm, TESForm, TESNPC);
    if(pNpc)
    {
        RequestServerAssignment(aRegistry, aEntity);
    }
}

void CharacterService::OnFormIdComponentRemoved(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    CancelServerAssignment(aRegistry, aEntity);
}

void CharacterService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunLocalUpdates();
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

void CharacterService::OnCharacterAssign(const TiltedMessages::CharacterAssignResponse& acMessage) noexcept
{
    auto view = m_world.view<WaitingForAssignmentComponent>();
    const auto itor = std::find_if(std::begin(view), std::end(view), [view, cookie = acMessage.cookie()](auto entity)
    {
        return view.get(entity).Cookie == cookie;
    });

    if (itor == std::end(view))
        return;

    if (acMessage.ownership())
    {
        m_world.emplace<LocalComponent>(*itor, acMessage.server_id());
        m_world.emplace<LocalAnimationComponent>(*itor);
    }
    else
    {
        m_world.emplace<RemoteComponent>(*itor, acMessage.server_id());

        const auto& formIdComponent = m_world.get<FormIdComponent>(*itor);

        const auto pForm = TESForm::GetById(formIdComponent.Id);
        auto pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            return;

        pActor->GetExtension()->SetRemote(true);

        InterpolationSystem::Setup(m_world, *itor);
        AnimationSystem::Setup(m_world, *itor);
    }
    
    m_world.remove<WaitingForAssignmentComponent>(*itor);
}

void CharacterService::OnCharacterSpawn(const TiltedMessages::CharacterSpawnRequest& acMessage) const noexcept
{
    // Temporary fix to load only custom forms
    if(!acMessage.has_game_id())
    {
        const auto cEntity = m_world.create();

        m_world.emplace<RemoteComponent>(cEntity, acMessage.server_id());

        InterpolationSystem::Setup(m_world, cEntity);
        AnimationSystem::Setup(m_world, cEntity);

        TESNPC* pNpc = nullptr;

        if(acMessage.has_base_id())
        {
            const auto cNpcId = World::Get().GetModSystem().GetGameId(acMessage.base_id().mod(), acMessage.base_id().base());
            if(cNpcId == 0)
            {
                m_world.destroy(cEntity);
                spdlog::error("Failed to retrieve NPC, it will not be spawned, possibly missing mod");
                return;
            }

            pNpc = RTTI_CAST(TESForm::GetById(cNpcId), TESForm, TESNPC);
            pNpc->Deserialize(acMessage.npc_buffer(), acMessage.change_flags());
        }
        else
        {
            pNpc = TESNPC::Create(acMessage.npc_buffer(), acMessage.change_flags());
            FaceGenSystem::Setup(m_world, cEntity);
        }

        auto pActor = Actor::Create(RTTI_CAST(pNpc, TESForm, TESNPC));

        pActor->GetExtension()->SetRemote(true);

        if(!acMessage.inventory_buffer().empty())
        {
        //    pActor->processManager->Deserialize(acMessage.inventory_buffer(), pActor);
        }

        spdlog::info("Actor created {:x}", reinterpret_cast<uintptr_t>(&(pActor->processManager->middleProcess->direction)));

        m_dispatcher.trigger(ReferenceSpawnedEvent(pActor->formID, pActor->formType, cEntity));
    }
}

void CharacterService::OnReferenceMovementSnapshot(const TiltedMessages::ReferenceMovementSnapshot& acMessage) noexcept
{
    auto view = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>();

    for (auto kvp : acMessage.movements())
    {
        auto itor = std::find_if(std::begin(view), std::end(view), [serverId = kvp.first, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == serverId;
        });

        if (itor == std::end(view))
            continue;

        auto& interpolationComponent = view.get<InterpolationComponent>(*itor);
        auto& movement = kvp.second;

        InterpolationComponent::TimePoint point;
        point.Tick = acMessage.tick();
        point.Position = Vector3<float>(movement.position().x(), movement.position().y(), movement.position().z());
        point.Rotation = Vector3<float>(movement.rotation().x(), movement.rotation().y(), movement.rotation().z());
        point.Speed = movement.speed();
        point.Direction = movement.direction();

        InterpolationSystem::AddPoint(interpolationComponent, point);
    }

    for (auto kvp : acMessage.actions())
    {
        auto itor = std::find_if(std::begin(view), std::end(view), [serverId = kvp.first, view](entt::entity entity)
        {
            return view.get<RemoteComponent>(entity).Id == serverId;
        });

        if (itor == std::end(view))
            continue;

        auto& animationComponent = view.get<RemoteAnimationComponent>(*itor);
        auto& actions = kvp.second;

        for(auto& action : actions.actions())
        {
            AnimationSystem::AddAction(animationComponent, action);
        }
    }
}

void CharacterService::OnActionEvent(const ActionEvent& acActionEvent) noexcept
{
    auto view = m_world.view<LocalAnimationComponent, FormIdComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acActionEvent.ActorId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == id;
    });

    if(itor != std::end(view))
    {
        auto& localComponent = view.get<LocalAnimationComponent>(*itor);
        auto outcome = localComponent.GetLatestAction();

        localComponent.Append(acActionEvent);
    }
}

void CharacterService::RequestServerAssignment(entt::registry& aRegistry, const entt::entity aEntity) const noexcept
{
    if (!m_transport.IsOnline())
        return;

    static uint32_t sCookieSeed = 0;

    const auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);

    const auto pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
    if (!pActor)
        return;

    const auto pNpc = RTTI_CAST(pActor->baseForm, TESForm, TESNPC);
    if (!pNpc)
        return;

    uint32_t baseId = 0;
    uint32_t modId = 0;
    if (!World::Get().GetModSystem().GetServerModId(formIdComponent.Id, modId, baseId))
        return;

    uint32_t cellBaseId = 0;
    uint32_t cellModId = 0;
    if (!World::Get().GetModSystem().GetServerModId(pActor->GetCellId(), cellModId, cellBaseId))
        return;

    TiltedMessages::ClientMessage message;
    auto pAssignmentRequest = message.mutable_character_assign_request();
    pAssignmentRequest->set_cookie(sCookieSeed);
    pAssignmentRequest->mutable_id()->set_base(baseId);
    pAssignmentRequest->mutable_cell_id()->set_base(cellBaseId);
    pAssignmentRequest->mutable_id()->set_mod(modId);
    pAssignmentRequest->mutable_cell_id()->set_mod(cellModId);
    pAssignmentRequest->mutable_cell_id()->set_mod(cellModId);

    auto pMovement = pAssignmentRequest->mutable_movement();
    pMovement->mutable_position()->set_x(pActor->position.x);
    pMovement->mutable_position()->set_y(pActor->position.y);
    pMovement->mutable_position()->set_z(pActor->position.z);

    pMovement->mutable_rotation()->set_x(pActor->rotation.x);
    pMovement->mutable_rotation()->set_y(pActor->rotation.y);
    pMovement->mutable_rotation()->set_z(pActor->rotation.z);

    pMovement->set_speed(pActor->GetSpeed());

    if(pActor->processManager && pActor->processManager->middleProcess)
        pMovement->set_direction(pActor->processManager->middleProcess->direction);
    
    const auto isPlayer = (formIdComponent.Id == 0x14);
    const auto isTemporary = pActor->formID >= 0xFF000000;

    if(isPlayer)
    {
        pNpc->MarkChanged(0x2000800);
    }

    const auto changeFlags = pNpc->GetChangeFlags();

    if(isPlayer || pNpc->formID >= 0xFF000000 || changeFlags != 0)
    {
        pAssignmentRequest->set_change_flags(changeFlags);
        pNpc->Serialize(pAssignmentRequest->mutable_npc_buffer());
    }

    //pActor->processManager->Serialize(pAssignmentRequest->mutable_inventory_buffer());

    if(isTemporary)
    {
        uint32_t baseNpcId = 0;
        uint32_t modNpcId = 0;
        if (!World::Get().GetModSystem().GetServerModId(pNpc->formID, modNpcId, baseNpcId))
            return;

        pAssignmentRequest->mutable_base_id()->set_base(baseNpcId);
        pAssignmentRequest->mutable_base_id()->set_mod(modNpcId);
    }

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

        TiltedMessages::ClientMessage message;
        auto pCancelRequest = message.mutable_cancel_character_assign_request();
        pCancelRequest->set_cookie(waitingComponent.Cookie);

        m_transport.Send(message);

        aRegistry.remove<WaitingForAssignmentComponent>(aEntity);
    }

    if (aRegistry.has<LocalComponent>(aEntity))
    {
        auto& localComponent = aRegistry.get<LocalComponent>(aEntity);

        TiltedMessages::ClientMessage message;
        auto pRemoveRequest = message.mutable_character_remove_request();
        pRemoveRequest->set_server_id(localComponent.Id);

        m_transport.Send(message);

        aRegistry.remove<LocalComponent>(aEntity);
    }

    if (aRegistry.has<RemoteComponent>(aEntity))
    {
        // Do nothing, a system will detect a remote component missing a form id component later on and try to spawn it
    }
}

void CharacterService::RunLocalUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 100ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    TiltedMessages::ClientMessage message;
    auto pSnapshot = message.mutable_reference_movement_snapshot();
    pSnapshot->set_tick(m_transport.GetClock().GetCurrentTick());

    auto pMovements = pSnapshot->mutable_movements();
    auto pActions = pSnapshot->mutable_actions();

    m_world.view<LocalComponent, LocalAnimationComponent, FormIdComponent>()
        .each([pMovements, pActions, this](auto entity, auto& localComponent, auto& animationComponent, auto& formIdComponent)
    {
        auto pForm = TESForm::GetById(formIdComponent.Id);
        const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
        if (!pActor)
            return;

        TiltedMessages::Movement movement;
        movement.mutable_position()->set_x(pActor->position.x);
        movement.mutable_position()->set_y(pActor->position.y);
        movement.mutable_position()->set_z(pActor->position.z);

        movement.mutable_rotation()->set_x(pActor->rotation.x);
        movement.mutable_rotation()->set_y(pActor->rotation.y);
        movement.mutable_rotation()->set_z(pActor->rotation.z);

        movement.set_speed(pActor->GetSpeed());
        if(pActor->processManager && pActor->processManager->middleProcess)
            movement.set_direction(pActor->processManager->middleProcess->direction);

        TiltedMessages::ActorActionData actorActions;
        for(auto& actionEvent : animationComponent.Actions)
        {
            uint32_t actionBaseId = 0;
            uint32_t actionModId = 0;
            if (!m_world.GetModSystem().GetServerModId(actionEvent.ActionId, actionModId, actionBaseId))
                return;

            // TODO: Send target id, have to resolve the server id first and then send it

            TiltedMessages::ActionData* pActionData = actorActions.add_actions();

            pActionData->mutable_action_id()->set_base(actionBaseId);
            pActionData->mutable_action_id()->set_mod(actionModId);
            pActionData->set_tick(actionEvent.Tick);
            pActionData->set_idle_id(actionEvent.IdleId);
            pActionData->set_state_1(actionEvent.State1);
            pActionData->set_state_2(actionEvent.State2);
            pActionData->set_value(actionEvent.Value);
            pActionData->set_type(actionEvent.Type);
            pActionData->mutable_variables()->Add(std::begin(actionEvent.Variables), std::end(actionEvent.Variables));
        }

        auto latestAction = animationComponent.GetLatestAction();

        if (latestAction)
            localComponent.CurrentAction = latestAction.MoveResult();

        animationComponent.Actions.clear();

        pMovements->operator[](localComponent.Id) = std::move(movement);

        if(!actorActions.actions().empty())
            pActions->operator[](localComponent.Id) = std::move(actorActions);
    });

    m_transport.Send(message);
}

void CharacterService::RunRemoteUpdates() noexcept
{
    // Delay by 120ms to let the interpolation system accumulate interpolation points
    auto tick = m_transport.GetClock().GetCurrentTick() - 120;

    m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, FormIdComponent>().each(
        [tick, this](auto entity, auto& remoteComponent, auto& interpolationComponent, auto& animationComponent, auto& formIdComponent)
        {
            auto pForm = TESForm::GetById(formIdComponent.Id);
            const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                return;

            InterpolationSystem::Update(pActor, interpolationComponent, tick);
            AnimationSystem::Update(m_world, pActor, animationComponent, tick);
        });

    m_world.view<FormIdComponent, FaceGenComponent>().each(
        [this](auto entity, auto& formIdComponent, auto& faceGenComponent)
        {
            auto pForm = TESForm::GetById(formIdComponent.Id);
            const auto pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                return;

            FaceGenSystem::Update(m_world, pActor, faceGenComponent);
        });

    m_world.group<RemoteComponent>(entt::exclude<FormIdComponent>).each([](auto entity, auto& remoteComponent)
        {
            // The entity has a remote component but no form id so we need to spawn it
            TP_UNUSED(entity);
            TP_UNUSED(remoteComponent);
        });
}
