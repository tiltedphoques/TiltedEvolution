#include <stdafx.h>

#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Events/CharacterSpawnedEvent.h>
#include <Events/CharacterCellChangeEvent.h>
#include <Events/PlayerEnterWorldEvent.h>
#include <Events/UpdateEvent.h>
#include <Scripts/Npc.h>

#include <Messages/AssignCharacterRequest.h>
#include <Messages/AssignCharacterResponse.h>
#include <Messages/ServerReferencesMoveRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/CharacterSpawnRequest.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/NotifyFactionsChanges.h>
#include <Messages/RemoveCharacterRequest.h>
#include <Messages/NotifyRemoveCharacter.h>
#include <Messages/CharacterTravelRequest.h>
#include <Messages/NotifyCharacterTravel.h>
#include <Messages/RequestSpawnData.h>
#include <Messages/NotifySpawnData.h>

CharacterService::CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&CharacterService::OnUpdate>(this))
    , m_characterCellChangeEventConnection(aDispatcher.sink<CharacterCellChangeEvent>().connect<&CharacterService::OnCharacterCellChange>(this))
    , m_characterAssignRequestConnection(aDispatcher.sink<PacketEvent<AssignCharacterRequest>>().connect<&CharacterService::OnAssignCharacterRequest>(this))
    , m_removeChatacterConnection(aDispatcher.sink<PacketEvent<RemoveCharacterRequest>>().connect<&CharacterService::OnRemoveCharacterRequest>(this))
    , m_characterSpawnedConnection(aDispatcher.sink<CharacterSpawnedEvent>().connect<&CharacterService::OnCharacterSpawned>(this))
    , m_referenceMovementSnapshotConnection(aDispatcher.sink<PacketEvent<ClientReferencesMoveRequest>>().connect<&CharacterService::OnReferencesMoveRequest>(this))
    , m_inventoryChangesConnection(aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&CharacterService::OnInventoryChanges>(this))
    , m_factionsChangesConnection(aDispatcher.sink<PacketEvent<RequestFactionsChanges>>().connect<&CharacterService::OnFactionsChanges>(this))
    , m_characterTravelConnection(aDispatcher.sink<PacketEvent<CharacterTravelRequest>>().connect<&CharacterService::OnCharacterTravel>(this))
    , m_spawnDataConnection(aDispatcher.sink<PacketEvent<RequestSpawnData>>().connect<&CharacterService::OnRequestSpawnData>(this))
{
}

void CharacterService::Serialize(const World& aRegistry, entt::entity aEntity, CharacterSpawnRequest* apSpawnRequest) noexcept
{
    const auto& characterComponent = aRegistry.get<CharacterComponent>(aEntity);

    apSpawnRequest->ServerId = World::ToInteger(aEntity);
    apSpawnRequest->AppearanceBuffer = characterComponent.SaveBuffer;
    apSpawnRequest->ChangeFlags = characterComponent.ChangeFlags;
    apSpawnRequest->FaceTints = characterComponent.FaceTints;
    apSpawnRequest->FactionsContent = characterComponent.FactionsContent;
    apSpawnRequest->IsDead = characterComponent.IsDead;

    const auto* pFormIdComponent = aRegistry.try_get<FormIdComponent>(aEntity);
    if (pFormIdComponent)
    {
        apSpawnRequest->FormId = pFormIdComponent->Id;
    }

    const auto* pInventoryComponent = aRegistry.try_get<InventoryComponent>(aEntity);
    if (pInventoryComponent)
    {
        apSpawnRequest->InventoryContent = pInventoryComponent->Content;
    }

    const auto* pActorValuesComponent = aRegistry.try_get<ActorValuesComponent>(aEntity);
    if (pActorValuesComponent)
    {
        apSpawnRequest->InitialActorValues = pActorValuesComponent->CurrentActorValues;
    }

    if (characterComponent.BaseId)
    {
        apSpawnRequest->BaseId = characterComponent.BaseId.Id;
    }

    const auto* pMovementComponent = aRegistry.try_get<MovementComponent>(aEntity);
    if (pMovementComponent)
    {
        apSpawnRequest->Position = pMovementComponent->Position;
        apSpawnRequest->Rotation.x = pMovementComponent->Rotation.x;
        apSpawnRequest->Rotation.y = pMovementComponent->Rotation.z;
    }

    const auto& animationComponent = aRegistry.get<AnimationComponent>(aEntity);
    apSpawnRequest->LatestAction = animationComponent.CurrentAction;
}

void CharacterService::OnUpdate(const UpdateEvent&) const noexcept
{
    ProcessInventoryChanges();
    ProcessFactionsChanges();
    ProcessMovementChanges();
}

void CharacterService::OnCharacterCellChange(const CharacterCellChangeEvent& acEvent) const noexcept
{
    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();

    CharacterSpawnRequest spawnMessage;
    Serialize(m_world, acEvent.Entity, &spawnMessage);

    NotifyRemoveCharacter removeMessage;
    removeMessage.ServerId = World::ToInteger(acEvent.Entity);

    for (auto entity : playerView)
    {
        auto& playerComponent = playerView.get<PlayerComponent>(entity);
        auto& cellIdComponent = playerView.get<CellIdComponent>(entity);

        if (acEvent.Owner == entity)
            continue;

        if (acEvent.OldCell == cellIdComponent.Cell)
            GameServer::Get()->Send(playerComponent.ConnectionId, removeMessage);
        else if (acEvent.NewCell == cellIdComponent.Cell)
            GameServer::Get()->Send(playerComponent.ConnectionId, spawnMessage);
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
        auto view = m_world.view<FormIdComponent, ActorValuesComponent, CharacterComponent>();

        const auto itor = std::find_if(std::begin(view), std::end(view), [view, refId](auto entity)
            {
                const auto& formIdComponent = view.get<FormIdComponent>(entity);

                return formIdComponent.Id == refId;
            });

        if (itor != std::end(view))
        {
            // This entity already has an owner
            spdlog::info("FormId: {:x}:{:x} is already managed", refId.ModId, refId.BaseId);

            const auto* pServer = GameServer::Get();

            auto& actorValuesComponent = view.get<ActorValuesComponent>(*itor);
            auto& characterComponent = view.get<CharacterComponent>(*itor);

            AssignCharacterResponse response;
            response.Cookie = message.Cookie;
            response.ServerId = World::ToInteger(*itor);
            response.Owner = false;
            response.AllActorValues = actorValuesComponent.CurrentActorValues;
            response.IsDead = characterComponent.IsDead;

            pServer->Send(acMessage.ConnectionId, response);
            return;
        }
    }

    // This entity has no owner create it
    CreateCharacter(acMessage);
}

void CharacterService::OnRemoveCharacterRequest(const PacketEvent<RemoveCharacterRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    const auto view = m_world.view<OwnerComponent, CharacterComponent, CellIdComponent>();
    const auto it = view.find(static_cast<entt::entity>(message.ServerId));
    if (it == view.end())
    {
        spdlog::warn("Client {:X} requested the removal of an entity that doesn't exist !", acMessage.ConnectionId);
        return;
    }

    const auto& characterOwnerComponent = view.get<OwnerComponent>(*it);
    if (characterOwnerComponent.ConnectionId != acMessage.ConnectionId)
    {
        spdlog::warn("Client {:X} requested the removal of an entity that they do not own !", acMessage.ConnectionId);
        return;  
    }

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();

    NotifyRemoveCharacter response;
    response.ServerId = World::ToInteger(*it);

    for (auto entity : playerView)
    {
        auto& playerComponent = playerView.get<PlayerComponent>(entity);

        if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId)
            continue;

        GameServer::Get()->Send(playerComponent.ConnectionId, response);
    }

    // For now just destroy the entity, in the future we might want to request a new host
    m_world.destroy(*it);
}

void CharacterService::OnCharacterSpawned(const CharacterSpawnedEvent& acEvent) const noexcept
{
    CharacterSpawnRequest message;
    Serialize(m_world, acEvent.Entity, &message);

    const auto& characterCellIdComponent = m_world.get<CellIdComponent>(acEvent.Entity);
    const auto& characterOwnerComponent = m_world.get<OwnerComponent>(acEvent.Entity);

    const auto view = m_world.view<PlayerComponent, CellIdComponent>(); 

    for (auto entity : view)
    {
        auto& playerComponent = view.get<PlayerComponent>(entity);
        auto& cellIdComponent = view.get<CellIdComponent>(entity);

        if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId || characterCellIdComponent.Cell != cellIdComponent.Cell)
            continue;

        GameServer::Get()->Send(playerComponent.ConnectionId, message);
    }
}

void CharacterService::OnReferencesMoveRequest(const PacketEvent<ClientReferencesMoveRequest>& acMessage) const noexcept
{
    auto view = m_world.view<OwnerComponent, AnimationComponent, MovementComponent>();

    auto& message = acMessage.Packet;

    for (auto& entry : message.Updates)
    {
        auto itor = view.find(static_cast<entt::entity>(entry.first));

        if (itor == std::end(view) || view.get<OwnerComponent>(*itor).ConnectionId != acMessage.ConnectionId)
            continue;

        Script::Npc npc(*itor, m_world);

        auto& movementComponent = view.get<MovementComponent>(*itor);
        auto& animationComponent = view.get<AnimationComponent>(*itor);

        movementComponent.Tick = message.Tick;

        const auto movementCopy = movementComponent;

        auto& update = entry.second;
        auto& movement = update.UpdatedMovement;

        movementComponent.Position = movement.Position;
        movementComponent.Rotation = glm::vec3(movement.Rotation.x, 0.f, movement.Rotation.y);
        movementComponent.Variables = movement.Variables;
        movementComponent.Direction = movement.Direction;

        auto [canceled, reason] = m_world.GetScriptService().HandleMove(npc);

        if (canceled)
        {
            movementComponent = movementCopy;
        }

        for (auto& action : update.ActionEvents)
        {
            //TODO: HandleAction
            //auto [canceled, reason] = apWorld->GetScriptServce()->HandleMove(acMessage.ConnectionId, kvp.first);

            animationComponent.CurrentAction = action;

            animationComponent.Actions.push_back(animationComponent.CurrentAction);
        }

        movementComponent.Sent = false;
    }
}

void CharacterService::OnRequestSpawnData(const PacketEvent<RequestSpawnData>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto view = m_world.view<ActorValuesComponent, InventoryComponent>();
    
    auto itor = view.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(view))
    {
        NotifySpawnData notifySpawnData;
        notifySpawnData.Id = message.Id;

        const auto* pActorValuesComponent = m_world.try_get<ActorValuesComponent>(*itor);
        if (pActorValuesComponent)
        {
            notifySpawnData.InitialActorValues = pActorValuesComponent->CurrentActorValues;
        }

        const auto* pInventoryComponent = m_world.try_get<InventoryComponent>(*itor);
        if (pInventoryComponent)
        {
            notifySpawnData.InitialInventory = pInventoryComponent->Content;
        }

        notifySpawnData.IsDead = false;
        const auto* pCharacterComponent = m_world.try_get<CharacterComponent>(*itor);
        if (pCharacterComponent)
        {
            notifySpawnData.IsDead = pCharacterComponent->IsDead;
        }

        GameServer::Get()->Send(acMessage.ConnectionId, notifySpawnData);
    }
}

void CharacterService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) const noexcept
{
    auto view = m_world.view<InventoryComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    for (auto& [id, inventory] : message.Changes)
    {
        auto itor = view.find(static_cast<entt::entity>(id));

        if (itor == std::end(view) || view.get<OwnerComponent>(*itor).ConnectionId != acMessage.ConnectionId)
            continue;

        auto& inventoryComponent = view.get<InventoryComponent>(*itor);
        inventoryComponent.Content = inventory;
        inventoryComponent.DirtyInventory = true;
    }
}

void CharacterService::OnFactionsChanges(const PacketEvent<RequestFactionsChanges>& acMessage) const noexcept
{
    auto view = m_world.view<CharacterComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    for (auto& [id, factions] : message.Changes)
    {
        auto itor = view.find(static_cast<entt::entity>(id));

        if (itor == std::end(view) || view.get<OwnerComponent>(*itor).ConnectionId != acMessage.ConnectionId)
            continue;

        auto& characterComponent = view.get<CharacterComponent>(*itor);
        characterComponent.FactionsContent = factions;
        characterComponent.DirtyFactions = true;
    }
}

void CharacterService::OnCharacterTravel(const PacketEvent<CharacterTravelRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    const auto view = m_world.view<OwnerComponent, CharacterComponent, CellIdComponent>();
    const auto it = view.find(static_cast<entt::entity>(message.ServerId));
    if (it == view.end())
    {
        spdlog::warn("Client {:X} requested travel of an entity that doesn't exist !", acMessage.ConnectionId);
        return;
    }

    const auto& characterOwnerComponent = view.get<OwnerComponent>(*it);
    if (characterOwnerComponent.ConnectionId != acMessage.ConnectionId)
    {
        spdlog::warn("Client {:X} requested travel of an entity that they do not own !", acMessage.ConnectionId);
        return;
    }

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();

    NotifyCharacterTravel response;
    response.ServerId = World::ToInteger(*it);
    response.CellId = message.CellId;
    response.Position = message.Position;

    for (auto entity : playerView)
    {
        auto& playerComponent = playerView.get<PlayerComponent>(entity);

        if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId)
            continue;

        GameServer::Get()->Send(playerComponent.ConnectionId, response);
    }

    // For now just destroy the entity, in the future we might want to request a new host
    m_world.destroy(*it);
}

void CharacterService::CreateCharacter(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    const auto gameId = message.ReferenceId;
    const auto baseId = message.FormId;

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

    auto* const pServer = GameServer::Get();

    m_world.emplace<ScriptsComponent>(cEntity);
    m_world.emplace<OwnerComponent>(cEntity, acMessage.ConnectionId);
    m_world.emplace<CellIdComponent>(cEntity, message.CellId);

    auto& characterComponent = m_world.emplace<CharacterComponent>(cEntity);
    characterComponent.ChangeFlags = message.ChangeFlags;
    characterComponent.SaveBuffer = std::move(message.AppearanceBuffer);
    characterComponent.BaseId = FormIdComponent(message.FormId);
    characterComponent.FaceTints = message.FaceTints;
    characterComponent.FactionsContent = message.FactionsContent;
    characterComponent.IsDead = message.IsDead;

    auto& inventoryComponent = m_world.emplace<InventoryComponent>(cEntity);
    inventoryComponent.Content = message.InventoryContent;

    auto& actorValuesComponent = m_world.emplace<ActorValuesComponent>(cEntity);
    actorValuesComponent.CurrentActorValues = message.AllActorValues;

    spdlog::info("FormId: {:x}:{:x} - NpcId: {:x}:{:x} assigned to {:x}", gameId.ModId, gameId.BaseId, baseId.ModId, baseId.BaseId, acMessage.ConnectionId);

    auto& movementComponent = m_world.emplace<MovementComponent>(cEntity);
    movementComponent.Tick = pServer->GetTick();
    movementComponent.Position = message.Position;
    movementComponent.Rotation = {message.Rotation.x, 0.f, message.Rotation.y};
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
            return playerView.get<PlayerComponent>(entity).ConnectionId == connectionId;
        });

        if (itor == std::end(playerView))
        {
            spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
            return;
        }

        const auto cPlayer = *itor;

        auto& playerComponent = playerView.get<PlayerComponent>(cPlayer);
        playerComponent.Character = cEntity;

        auto& questLogComponent = m_world.emplace<QuestLogComponent>(cPlayer);
        questLogComponent.QuestContent = message.QuestContent;

        auto& dispatcher = m_world.GetDispatcher();
        dispatcher.trigger(PlayerEnterWorldEvent(cPlayer));
    }

    AssignCharacterResponse response;
    response.Cookie = message.Cookie;
    response.ServerId = World::ToInteger(cEntity);
    response.Owner = true;
    response.AllActorValues = message.AllActorValues;

    pServer->Send(acMessage.ConnectionId, response);

    auto& dispatcher = m_world.GetDispatcher();
    dispatcher.trigger(CharacterSpawnedEvent(cEntity));
}

void CharacterService::ProcessInventoryChanges() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 4;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto characterView = m_world.view < CellIdComponent, InventoryComponent, OwnerComponent >();

    Map<ConnectionId_t, NotifyInventoryChanges> messages;

    for (auto entity : characterView)
    {
        auto& inventoryComponent = characterView.get<InventoryComponent>(entity);
        auto& cellIdComponent = characterView.get<CellIdComponent>(entity);
        auto& ownerComponent = characterView.get<OwnerComponent>(entity);

        // If we have nothing new to send skip this
        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerView.get<CellIdComponent>(player) != cellIdComponent || playerComponent.ConnectionId == ownerComponent.ConnectionId)
                continue;

            auto& message = messages[playerComponent.ConnectionId];
            auto& change = message.Changes[World::ToInteger(entity)];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [connectionId, message] : messages)
    {
        if (!message.Changes.empty())
            GameServer::Get()->Send(connectionId, message);
    }
}

void CharacterService::ProcessFactionsChanges() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 2000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    const auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto characterView = m_world.view < CellIdComponent, CharacterComponent, OwnerComponent>();

    Map<ConnectionId_t, NotifyFactionsChanges> messages;

    for (auto entity : characterView)
    {
        auto& characterComponent = characterView.get<CharacterComponent>(entity);
        auto& cellIdComponent = characterView.get<CellIdComponent>(entity);
        auto& ownerComponent = characterView.get<OwnerComponent>(entity);

        // If we have nothing new to send skip this
        if (characterComponent.DirtyFactions == false)
            continue;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerView.get<CellIdComponent>(player) != cellIdComponent || playerComponent.ConnectionId == ownerComponent.ConnectionId)
                continue;

            auto& message = messages[playerComponent.ConnectionId];
            auto& change = message.Changes[World::ToInteger(entity)];

            change = characterComponent.FactionsContent;
        }

        characterComponent.DirtyFactions = false;
    }

    for (auto [connectionId, message] : messages)
    {
        if (!message.Changes.empty())
            GameServer::Get()->Send(connectionId, message);
    }
}

void CharacterService::ProcessMovementChanges() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 50;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    auto playerView = m_world.view<PlayerComponent, CellIdComponent>();
    const auto characterView = m_world.view < CellIdComponent, MovementComponent, AnimationComponent, OwnerComponent >();

    Map<ConnectionId_t, ServerReferencesMoveRequest> messages;

    for (auto player : playerView)
    {
        const auto& playerComponent = playerView.get<PlayerComponent>(player);
        auto& message = messages[playerComponent.ConnectionId];

        message.Tick = GameServer::Get()->GetTick();
    }

    for (auto entity : characterView)
    {
        auto& movementComponent = characterView.get<MovementComponent>(entity);
        auto& cellIdComponent = characterView.get<CellIdComponent>(entity);
        auto& ownerComponent = characterView.get<OwnerComponent>(entity);
        auto& animationComponent = characterView.get<AnimationComponent>(entity);

        // If we have nothing new to send skip this
        if (movementComponent.Sent == true)
            continue;

        for (auto player : playerView)
        {
            const auto& playerComponent = playerView.get<PlayerComponent>(player);

            if (playerView.get<CellIdComponent>(player) != cellIdComponent || playerComponent.ConnectionId == ownerComponent.ConnectionId)
                continue;

            auto& message = messages[playerComponent.ConnectionId];
            auto& update = message.Updates[World::ToInteger(entity)];
            auto& movement = update.UpdatedMovement;

            movement.Position = movementComponent.Position;

            movement.Rotation.x = movementComponent.Rotation.x;
            movement.Rotation.y = movementComponent.Rotation.z;

            movement.Direction = movementComponent.Direction;
            movement.Variables = movementComponent.Variables;

            update.ActionEvents = animationComponent.Actions;
        }
    }

    m_world.view<AnimationComponent>().each([](AnimationComponent& animationComponent)
        {
            if (!animationComponent.Actions.empty())
                animationComponent.LastSerializedAction = animationComponent.Actions[animationComponent.Actions.size() - 1];

            animationComponent.Actions.clear();
        });

    m_world.view<MovementComponent>().each([](MovementComponent& movementComponent)
        {
            movementComponent.Sent = true;
        });

    for (auto [connectionId, message] : messages)
    {
        if (!message.Updates.empty())
            GameServer::Get()->Send(connectionId, message);
    }
}
