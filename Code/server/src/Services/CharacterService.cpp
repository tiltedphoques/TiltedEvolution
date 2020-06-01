#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Events/CharacterSpawnedEvent.h>
#include <Events/UpdateEvent.h>
#include <Scripts/Npc.h>
#include <Scripts/Player.h>

CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&CharacterService::OnUpdate>(this))
    , m_characterAssignRequestConnection(aDispatcher.sink<PacketEvent<TiltedMessages::CharacterAssignRequest>>().connect<&CharacterService::OnCharacterAssignRequest>(this))
    , m_characterSpawnedConnection(aDispatcher.sink<CharacterSpawnedEvent>().connect<&CharacterService::OnCharacterSpawned>(this))
    , m_referenceMovementSnapshotConnection(aDispatcher.sink<PacketEvent<TiltedMessages::ReferenceMovementSnapshot>>().connect<&CharacterService::OnReferenceMovementSnapshot>(this))
{
}

void CharacterService::Serialize(const World& aRegistry, entt::entity aEntity, TiltedMessages::CharacterSpawnRequest* apSpawnRequest) noexcept
{
    const auto& characterComponent = aRegistry.get<CharacterComponent>(aEntity);

    apSpawnRequest->set_server_id(World::ToInteger(aEntity));
    apSpawnRequest->mutable_npc_buffer()->assign(characterComponent.SaveBuffer.c_str(), characterComponent.SaveBuffer.size());
    apSpawnRequest->mutable_inventory_buffer()->assign(characterComponent.InventoryBuffer.c_str(), characterComponent.InventoryBuffer.size());
    apSpawnRequest->set_change_flags(characterComponent.ChangeFlags);

    const auto* pFormIdComponent = aRegistry.try_get<FormIdComponent>(aEntity);
    if (pFormIdComponent)
    {
        apSpawnRequest->mutable_game_id()->set_mod(pFormIdComponent->ModId);
        apSpawnRequest->mutable_game_id()->set_base(pFormIdComponent->BaseId);
    }

    if(characterComponent.BaseId)
    {
        apSpawnRequest->mutable_base_id()->set_mod(characterComponent.BaseId.ModId);
        apSpawnRequest->mutable_base_id()->set_base(characterComponent.BaseId.BaseId);
    }

    const auto& animationComponent = aRegistry.get<AnimationComponent>(aEntity);
    *(apSpawnRequest->mutable_current_variables()) = animationComponent.CurrentVariables;
    *(apSpawnRequest->mutable_current_action()) = animationComponent.CurrentAction;
}

void CharacterService::OnUpdate(const UpdateEvent&) noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 50;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto characterView = m_world.view < CellIdComponent, MovementComponent, AnimationComponent, OwnerComponent > ();

    Map<ConnectionId_t, TiltedMessages::ServerMessage> messages;

    for (auto player : playerView)
    {
        const auto& playerComponent = playerView.get<PlayerComponent>(player);
        auto& message = messages[playerComponent.ConnectionId];

        auto pSnapshot = message.mutable_reference_movement_snapshot();
        pSnapshot->set_tick(GameServer::Get()->GetTick());
    }

    characterView.each([this, playerView, &messages](auto entity, const auto& cellIdComponent, const auto& movementComponent, const auto& animationComponent, const auto& ownerComponent)
    {
        // If we have nothing new to send skip this
        if (movementComponent.Sent == true)
            return;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerView.get<CellIdComponent>(player) != cellIdComponent ||
                playerComponent.ConnectionId == ownerComponent.ConnectionId)
                continue;

            auto& message = messages[playerComponent.ConnectionId];
            auto pSnapshot = message.mutable_reference_movement_snapshot()->add_entries();
            auto& movement = *pSnapshot->mutable_movement();
            auto& actions = *pSnapshot->mutable_actions();

            pSnapshot->set_server_id(World::ToInteger(entity));

            float x, y, z;
            movementComponent.Position.Decompose(x, y, z);

            movement.mutable_position()->set_x(x);
            movement.mutable_position()->set_y(y);
            movement.mutable_position()->set_z(z);

            movementComponent.Rotation.Decompose(x, y, z);

            movement.mutable_rotation()->set_x(x);
            movement.mutable_rotation()->set_z(z);

            if(!animationComponent.Actions.empty())
            {
                for(auto& action : animationComponent.Actions)
                {
                    *actions.add_actions() = action;
                }
            }
        }
    });

    m_world.view<AnimationComponent>().each([](auto entity, auto& animationComponent)
    {
        animationComponent.Actions.clear();
    });

    m_world.view<MovementComponent>().each([](auto entity, auto& movementComponent)
    {
        movementComponent.Sent = true;
    });

    for(auto kvp : messages)
    {
        if(kvp.second.reference_movement_snapshot().entries_size() > 0)
            GameServer::Get()->Send(kvp.first, kvp.second);
    }
}

void CharacterService::OnCharacterAssignRequest(const PacketEvent<TiltedMessages::CharacterAssignRequest>& acMessage) const noexcept
{
    const auto gameId = acMessage.Packet.id();

    const auto isCustom = (gameId.mod() == 0 && gameId.base() == 0x14) || gameId.mod() == std::numeric_limits<uint32_t>::max();

    // Check if id is the player
    if (!isCustom)
    {
        // Look for the character
        auto view = m_world.view<FormIdComponent>();

        const auto itor = std::find_if(std::begin(view), std::end(view), [view, &gameId](auto entity)
        {
            const auto& formIdComponent = view.get(entity);

            return formIdComponent.BaseId == gameId.base() && formIdComponent.ModId == gameId.mod();
        });

        if (itor != std::end(view))
        {
            // This entity already has an owner
            spdlog::info("FormId: {:x}:{:x} is already managed", gameId.mod(), gameId.base());

            const auto pServer = GameServer::Get();

            TiltedMessages::ServerMessage message;
            auto pResponse = message.mutable_character_assign_response();

            pResponse->set_cookie(acMessage.Packet.cookie());
            pResponse->set_server_id(World::ToInteger(*itor));
            pResponse->set_ownership(false);

            pServer->Send(acMessage.ConnectionId, message);

            return;
        }
    }

    // This entity has no owner create it
    CreateCharacter(acMessage);
}

void CharacterService::OnCharacterSpawned(const CharacterSpawnedEvent& acEvent) noexcept
{
    TiltedMessages::ServerMessage message;
    const auto pRequest = message.mutable_character_spawn_request();
    Serialize(m_world, acEvent.Entity, pRequest);

    const auto& characterCellIdComponent = m_world.get<CellIdComponent>(acEvent.Entity);
    const auto& characterOwnerComponent = m_world.get<OwnerComponent>(acEvent.Entity);

    m_world.view<PlayerComponent, CellIdComponent>().each(
        [&message, &characterCellIdComponent, &characterOwnerComponent](
        auto entity, const auto& playerComponent, const auto& cellIdComponent)
    {
        if(characterOwnerComponent.ConnectionId)

        if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId || characterCellIdComponent.CellId != cellIdComponent.CellId)
            return;

        GameServer::Get()->Send(playerComponent.ConnectionId, message);
    });
}

void CharacterService::OnReferenceMovementSnapshot(
    const PacketEvent<TiltedMessages::ReferenceMovementSnapshot>& acMessage) const noexcept
{
    auto view = m_world.view<CharacterComponent, OwnerComponent, AnimationComponent, MovementComponent>();

    for (auto& entry : acMessage.Packet.entries())
    {
        auto itor = view.find(entt::entity(entry.server_id()));

        if (itor == std::end(view) || view.get<OwnerComponent>(*itor).ConnectionId != acMessage.ConnectionId)
            continue;

        Script::Npc npc(*itor, m_world);

        auto& movementComponent = view.get<MovementComponent>(*itor);
        auto& animationComponent = view.get<AnimationComponent>(*itor);

        movementComponent.Tick = acMessage.Packet.tick();

        auto movementCopy = movementComponent;

        auto& movement = entry.movement();

        movementComponent.Position = Vector3<float>(movement.position().x(), movement.position().y(), movement.position().z());
        movementComponent.Rotation = Vector3<float>(movement.rotation().x(), 0.f, movement.rotation().z());

        auto [canceled, reason] = m_world.GetScriptService().HandleMove(npc);

        if(canceled)
        {
            movementComponent = movementCopy;
        }

        for (auto& action : entry.actions().actions())
        {
            //TODO: HandleAction
            //auto [canceled, reason] = apWorld->GetScriptServce()->HandleMove(acMessage.ConnectionId, kvp.first);

            animationComponent.Actions.push_back(action);
        }

        if (!animationComponent.Actions.empty())
            animationComponent.CurrentVariables = animationComponent.Actions[animationComponent.Actions.size() - 1];

        movementComponent.Sent = false;
    }
}

void CharacterService::CreateCharacter(const PacketEvent<TiltedMessages::CharacterAssignRequest>& acMessage) const noexcept
{
    const auto gameId = acMessage.Packet.id();
    const auto baseId = acMessage.Packet.base_id();

    spdlog::info("FormId: {:x}:{:x} - NpcId: {:x}:{:x}  assigned to {:x}", gameId.mod(), gameId.base(), baseId.mod(), baseId.base(), acMessage.ConnectionId);

    const auto cEntity = m_world.create();
    const auto isTemporary = gameId.mod() == std::numeric_limits<uint32_t>::max();
    const auto isPlayer = (gameId.mod() == 0 && gameId.base() == 0x14);
    const auto isCustom = isPlayer || isTemporary;

    // For player characters and temporary forms
    if (!isCustom)
    {
        m_world.emplace<FormIdComponent>(cEntity, gameId.base(), gameId.mod());
    }
    else if(acMessage.Packet.has_base_id() && !isTemporary)
    {
        spdlog::warn("Unexpected NpcId, player {:x} might be forging packets", acMessage.ConnectionId);
        return;
    }

    auto& packet = acMessage.Packet;
    const auto pServer = GameServer::Get();

    m_world.emplace<ScriptsComponent>(cEntity);
    m_world.emplace<OwnerComponent>(cEntity, acMessage.ConnectionId);
    m_world.emplace<CellIdComponent>(cEntity, packet.cell_id());

    CharacterComponent& characterComponent = m_world.emplace<CharacterComponent>(cEntity);
    characterComponent.ChangeFlags = packet.change_flags();
    characterComponent.SaveBuffer.assign(packet.npc_buffer().c_str(), packet.npc_buffer().size());
    characterComponent.InventoryBuffer.assign(packet.inventory_buffer().c_str(), packet.inventory_buffer().size());
    characterComponent.BaseId = FormIdComponent(packet.base_id());

    MovementComponent& movementComponent = m_world.emplace<MovementComponent>(cEntity);
    movementComponent.Tick = pServer->GetTick();
    movementComponent.Position = Vector3<float>(packet.movement().position().x(), packet.movement().position().y(), packet.movement().position().z());
    movementComponent.Rotation = Vector3<float>(packet.movement().rotation().x(), 0.f, packet.movement().rotation().z());
    movementComponent.Sent = false;

    auto& animationComponent = m_world.emplace<AnimationComponent>(cEntity);
    animationComponent.CurrentVariables = packet.current_variables();
    animationComponent.CurrentAction = packet.current_action();

    // If this is a player character store a ref and trigger an event
    if (isPlayer)
    {
        auto playerView = m_world.view<PlayerComponent>();

        const auto itor = std::find_if(std::begin(playerView), std::end(playerView),
            [playerView, connectionId = acMessage.ConnectionId](auto entity)
        {
            return playerView.get(entity).ConnectionId == connectionId;
        });

        if (itor == std::end(playerView))
        {
            spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
            return;
        }

        const auto cPlayer = *itor;

        auto& playerComponent = playerView.get<PlayerComponent>(cPlayer);
        playerComponent.Character = cEntity;

        Script::Player player(cPlayer, m_world);
        m_world.GetScriptService().HandlePlayerEnterWorld(player);
    }

    TiltedMessages::ServerMessage message;
    auto pResponse = message.mutable_character_assign_response();
    pResponse->set_cookie(acMessage.Packet.cookie());
    pResponse->set_server_id(World::ToInteger(cEntity));
    pResponse->set_ownership(true);

    pServer->Send(acMessage.ConnectionId, message);

    auto& dispatcher = m_world.GetDispatcher();
    dispatcher.trigger(CharacterSpawnedEvent(cEntity));
}
