#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Events/CharacterSpawnedEvent.h>
#include <Events/UpdateEvent.h>
#include <Scripts/Npc.h>
#include <Scripts/Player.h>

#include <Messages/AssignCharacterRequest.h>
#include <Messages/AssignCharacterResponse.h>

CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&CharacterService::OnUpdate>(this))
    , m_characterAssignRequestConnection(aDispatcher.sink<PacketEvent<AssignCharacterRequest>>().connect<&CharacterService::OnAssignCharacterRequest>(this))
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

    if (characterComponent.BaseId)
    {
        apSpawnRequest->mutable_base_id()->set_mod(characterComponent.BaseId.ModId);
        apSpawnRequest->mutable_base_id()->set_base(characterComponent.BaseId.BaseId);
    }

    uint8_t scratch[1 << 12];

    ViewBuffer buffer(scratch, std::size(scratch));
    Buffer::Writer writer(&buffer);

    const auto& animationComponent = aRegistry.get<AnimationComponent>(aEntity);
    animationComponent.CurrentAction.GenerateDifferential(ActionEvent{}, writer);

    apSpawnRequest->mutable_current_action()->assign(buffer.GetData(), buffer.GetData() + writer.GetBytePosition());
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
    const auto characterView = m_world.view < CellIdComponent, MovementComponent, AnimationComponent, OwnerComponent >();

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

                auto lastSerializedAction = animationComponent.LastSerializedAction;

                if (!animationComponent.Actions.empty())
                {
                    for (auto& action : animationComponent.Actions)
                    {
                        uint8_t scratch[1 << 12];

                        ViewBuffer buffer(scratch, std::size(scratch));
                        Buffer::Writer writer(&buffer);

                        action.GenerateDifferential(lastSerializedAction, writer);
                        lastSerializedAction = action;

                        actions.add_actions()->assign(buffer.GetData(), buffer.GetData() + writer.GetBytePosition());
                    }
                }
            }
        });

    m_world.view<AnimationComponent>().each([](auto entity, auto& animationComponent)
        {
            if (!animationComponent.Actions.empty())
                animationComponent.LastSerializedAction = animationComponent.Actions[animationComponent.Actions.size() - 1];

            animationComponent.Actions.clear();
        });

    m_world.view<MovementComponent>().each([](auto entity, auto& movementComponent)
        {
            movementComponent.Sent = true;
        });

    for (auto kvp : messages)
    {
       // if (kvp.second.reference_movement_snapshot().entries_size() > 0)
       //     GameServer::Get()->Send(kvp.first, kvp.second);
    }
}

void CharacterService::OnAssignCharacterRequest(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;
    const auto& refId = message.ReferenceId;

    const auto isCustom = (refId.ModId == 0 && refId.BaseId == 0x14) || refId.ModId == std::numeric_limits<uint32_t>::max();

    // Check if id is the player
    if (!isCustom)
    {
        // Look for the character
        auto view = m_world.view<FormIdComponent>();

        const auto itor = std::find_if(std::begin(view), std::end(view), [view, refId](auto entity)
            {
                const auto& formIdComponent = view.get(entity);

                return formIdComponent.BaseId == refId.BaseId && formIdComponent.ModId == refId.ModId;
            });

        if (itor != std::end(view))
        {
            // This entity already has an owner
            spdlog::info("FormId: {:x}:{:x} is already managed", refId.ModId, refId.BaseId);

            const auto pServer = GameServer::Get();

            AssignCharacterResponse response;
            response.Cookie = message.Cookie;
            response.ServerId = World::ToInteger(*itor);
            response.Owner = false;

            pServer->Send(acMessage.ConnectionId, response);

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
            if (characterOwnerComponent.ConnectionId)

                if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId || characterCellIdComponent.CellId != cellIdComponent.CellId)
                    return;

           // GameServer::Get()->Send(playerComponent.ConnectionId, message);
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

        const auto movementCopy = movementComponent;

        auto& movement = entry.movement();

        movementComponent.Position = Vector3<float>(movement.position().x(), movement.position().y(), movement.position().z());
        movementComponent.Rotation = Vector3<float>(movement.rotation().x(), 0.f, movement.rotation().z());

        auto [canceled, reason] = m_world.GetScriptService().HandleMove(npc);

        if (canceled)
        {
            movementComponent = movementCopy;
        }

        for (auto& action : entry.actions().actions())
        {
            //TODO: HandleAction
            //auto [canceled, reason] = apWorld->GetScriptServce()->HandleMove(acMessage.ConnectionId, kvp.first);

            ViewBuffer buffer((uint8_t*)action.data(), action.size());
            Buffer::Reader reader(&buffer);
            animationComponent.CurrentAction.ApplyDifferential(reader);

            animationComponent.Actions.push_back(animationComponent.CurrentAction);
        }

        movementComponent.Sent = false;
    }
}

void CharacterService::CreateCharacter(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    const auto gameId = message.ReferenceId;
    const auto baseId = message.FormId;

    spdlog::info("FormId: {:x}:{:x} - NpcId: {:x}:{:x}  assigned to {:x}", gameId.ModId, gameId.BaseId, baseId.ModId, baseId.BaseId, acMessage.ConnectionId);

    const auto cEntity = m_world.create();
    const auto isTemporary = gameId.ModId == std::numeric_limits<uint32_t>::max();
    const auto isPlayer = (gameId.ModId == 0 && gameId.BaseId == 0x14);
    const auto isCustom = isPlayer || isTemporary;

    // For player characters and temporary forms
    if (!isCustom)
    {
        m_world.emplace<FormIdComponent>(cEntity, gameId.BaseId, gameId.ModId);
    }
    else if (baseId != GameId{} && !isTemporary)
    {
        spdlog::warn("Unexpected NpcId, player {:x} might be forging packets", acMessage.ConnectionId);
        return;
    }

    const auto pServer = GameServer::Get();

    m_world.emplace<ScriptsComponent>(cEntity);
    m_world.emplace<OwnerComponent>(cEntity, acMessage.ConnectionId);
    m_world.emplace<CellIdComponent>(cEntity, message.CellId);

    CharacterComponent& characterComponent = m_world.emplace<CharacterComponent>(cEntity);
    characterComponent.ChangeFlags = message.ChangeFlags;
    characterComponent.SaveBuffer = std::move(message.AppearanceBuffer);
    //characterComponent.InventoryBuffer = std::move(message.InventoryBuffer);
    characterComponent.BaseId = FormIdComponent(message.FormId);

    MovementComponent& movementComponent = m_world.emplace<MovementComponent>(cEntity);
    movementComponent.Tick = pServer->GetTick();
    movementComponent.Position = Vector3<float>(message.Position.X, message.Position.Y, message.Position.Z);
    movementComponent.Rotation = Vector3<float>(message.Rotation.X, 0.f, message.Rotation.Y);
    movementComponent.Sent = false;

    auto& animationComponent = m_world.emplace<AnimationComponent>(cEntity);

    animationComponent.CurrentAction = message.LatestAction;

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

        const Script::Player player(cPlayer, m_world);
        m_world.GetScriptService().HandlePlayerEnterWorld(player);
    }

    AssignCharacterResponse response;
    response.Cookie = message.Cookie;
    response.ServerId = World::ToInteger(cEntity);
    response.Owner = true;

    pServer->Send(acMessage.ConnectionId, response);

    auto& dispatcher = m_world.GetDispatcher();
    dispatcher.trigger(CharacterSpawnedEvent(cEntity));
}
