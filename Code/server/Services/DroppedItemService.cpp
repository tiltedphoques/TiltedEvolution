#include <Services/DroppedItemService.h>

#include <GameServer.h>
#include <World.h>
#include <Components.h>

#include <Events/UpdateEvent.h>
#include <Events/PlayerLeaveEvent.h>

#include <Messages/DropItemRequest.h>
#include <Messages/DropItemResponse.h>
#include <Messages/DroppedItemMoveRequest.h>
#include <Messages/PickUpDroppedItemRequest.h>
#include <Messages/NotifyDroppedItemsSpawn.h>
#include <Messages/NotifyDroppedItemMove.h>
#include <Messages/NotifyDroppedItemsRemove.h>

#include <Setting.h>

namespace
{
Console::Setting uMaxDroppedItems{"Gameplay:uMaxDroppedItems", "Maximum number of dropped items the server keeps. When the limit is reached, the oldest items disappear", 1000u};

constexpr float cInterestUpdateInterval = 0.25f;

CellIdComponent MakeCellComponent(const GameId& acCellId, const GameId& acWorldSpaceId, const DroppedItemTransform& acTransform) noexcept
{
    if (!acWorldSpaceId)
        return CellIdComponent{acCellId, {}, {}};

    return CellIdComponent{acCellId, acWorldSpaceId, GridCellCoords::CalculateGridCellCoords(acTransform.Position)};
}
} // namespace

DroppedItemService::DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&DroppedItemService::OnUpdate>(this);
    m_playerLeaveConnection = aDispatcher.sink<PlayerLeaveEvent>().connect<&DroppedItemService::OnPlayerLeave>(this);
    m_dropItemConnection = aDispatcher.sink<PacketEvent<DropItemRequest>>().connect<&DroppedItemService::OnDropItem>(this);
    m_droppedItemMoveConnection = aDispatcher.sink<PacketEvent<DroppedItemMoveRequest>>().connect<&DroppedItemService::OnDroppedItemMove>(this);
    m_pickUpDroppedItemConnection = aDispatcher.sink<PacketEvent<PickUpDroppedItemRequest>>().connect<&DroppedItemService::OnPickUpDroppedItem>(this);
}

void DroppedItemService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    m_timeSinceInterestUpdate += acEvent.Delta;
    if (m_timeSinceInterestUpdate < cInterestUpdateInterval)
        return;

    m_timeSinceInterestUpdate = 0.f;
    UpdateInterest();
}

void DroppedItemService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    m_knownItems.erase(acEvent.pPlayer);

    auto view = m_world.view<DroppedItemComponent>();
    for (auto entity : view)
    {
        auto& droppedItem = view.get<DroppedItemComponent>(entity);
        if (droppedItem.pSimulator == acEvent.pPlayer)
            StopSimulation(entity, droppedItem);
    }
}

void DroppedItemService::OnDropItem(const PacketEvent<DropItemRequest>& acMessage) noexcept
{
    const auto& message = acMessage.Packet;
    Player* pPlayer = acMessage.pPlayer;

    DropItemResponse response{};
    response.LocalId = message.LocalId;

    // A zero server id tells the client to stop tracking the item.
    if (!GameServer::Get()->AllowsItemDrops() || message.Item.Count <= 0 || !pPlayer->GetCellComponent())
    {
        pPlayer->Send(response);
        return;
    }

    EvictOldestItems();

    const auto entity = m_world.create();

    auto& droppedItem = m_world.emplace<DroppedItemComponent>(entity);
    droppedItem.Item = message.Item;
    droppedItem.Transform = message.Transform;
    droppedItem.pSimulator = pPlayer;
    droppedItem.Sequence = m_nextSequence++;

    m_world.emplace<CellIdComponent>(entity, MakeCellComponent(message.CellId, message.WorldSpaceId, message.Transform));

    // The dropping client already has its own reference.
    m_knownItems[pPlayer].insert(entity);

    response.ServerId = World::ToInteger(entity);
    pPlayer->Send(response);

    spdlog::debug("Player {:X} dropped item {:X}:{:X} (count {}), server id {:X}", pPlayer->GetId(), message.Item.BaseId.ModId, message.Item.BaseId.BaseId, message.Item.Count, response.ServerId);

    // Spawn it for the players in range right away instead of waiting for the next interest update.
    UpdateInterest();
}

void DroppedItemService::OnDroppedItemMove(const PacketEvent<DroppedItemMoveRequest>& acMessage) noexcept
{
    const auto& move = acMessage.Packet.Move;

    auto view = m_world.view<DroppedItemComponent, CellIdComponent>();
    const auto it = view.find(static_cast<entt::entity>(move.ServerId));
    if (it == view.end())
        return;

    auto& droppedItem = view.get<DroppedItemComponent>(*it);
    if (droppedItem.pSimulator != acMessage.pPlayer)
    {
        spdlog::debug("Ignored dropped item move from player {:X} for item {:X}, which it does not simulate", acMessage.pPlayer->GetId(), move.ServerId);
        return;
    }

    droppedItem.Transform = move.Transform;

    // An exterior item can fall into another cell; keep its range checks up to date.
    auto& cellComponent = view.get<CellIdComponent>(*it);
    if (!cellComponent.IsInInteriorCell())
        cellComponent.CenterCoords = GridCellCoords::CalculateGridCellCoords(move.Transform.Position);

    if (move.IsAtRest)
        droppedItem.pSimulator = nullptr;

    NotifyDroppedItemMove notify{};
    notify.Move = move;

    SendToPlayersWithItem(notify, *it, acMessage.pPlayer);
}

void DroppedItemService::OnPickUpDroppedItem(const PacketEvent<PickUpDroppedItemRequest>& acMessage) noexcept
{
    const auto entity = static_cast<entt::entity>(acMessage.Packet.ServerId);

    auto view = m_world.view<DroppedItemComponent>();
    if (view.find(entity) == view.end())
    {
        // Another player picked it up first. The first pickup wins; this one is not relayed.
        spdlog::debug("Player {:X} picked up dropped item {:X}, which is already gone", acMessage.pPlayer->GetId(), acMessage.Packet.ServerId);
        return;
    }

    RemoveItem(entity, acMessage.pPlayer);
}

void DroppedItemService::UpdateInterest() noexcept
{
    auto view = m_world.view<DroppedItemComponent, CellIdComponent>();

    for (Player* pPlayer : m_world.GetPlayerManager())
    {
        const auto& playerCell = pPlayer->GetCellComponent();
        if (!playerCell)
            continue;

        auto& knownItems = m_knownItems[pPlayer];

        NotifyDroppedItemsSpawn spawn{};
        NotifyDroppedItemsRemove remove{};

        for (auto entity : view)
        {
            const bool isInRange = playerCell.IsInRange(view.get<CellIdComponent>(entity), false);
            const bool isKnown = knownItems.contains(entity);

            if (isInRange && !isKnown)
            {
                knownItems.insert(entity);
                spawn.Items.push_back(ToData(entity));
            }
            else if (!isInRange && isKnown)
            {
                knownItems.erase(entity);
                remove.ServerIds.push_back(World::ToInteger(entity));

                // The client deletes its copy, so it can no longer stream the physics.
                auto& droppedItem = view.get<DroppedItemComponent>(entity);
                if (droppedItem.pSimulator == pPlayer)
                    StopSimulation(entity, droppedItem);
            }
        }

        if (!spawn.Items.empty())
            pPlayer->Send(spawn);

        if (!remove.ServerIds.empty())
            pPlayer->Send(remove);
    }
}

void DroppedItemService::EvictOldestItems() noexcept
{
    const auto maxItems = uMaxDroppedItems.value_as<uint32_t>();

    auto view = m_world.view<DroppedItemComponent>();
    while (!view.empty() && view.size() >= maxItems)
    {
        const auto oldest = *std::min_element(
            view.begin(), view.end(),
            [view](entt::entity aLhs, entt::entity aRhs) { return view.get<DroppedItemComponent>(aLhs).Sequence < view.get<DroppedItemComponent>(aRhs).Sequence; });

        spdlog::debug("Evicting dropped item {:X}, the limit of {} dropped items was reached", World::ToInteger(oldest), maxItems);
        RemoveItem(oldest, nullptr);
    }
}

void DroppedItemService::RemoveItem(entt::entity aEntity, const Player* apExcludedPlayer) noexcept
{
    NotifyDroppedItemsRemove notify{};
    notify.ServerIds.push_back(World::ToInteger(aEntity));

    // hopscotch_map only exposes mutable values through the iterator.
    for (auto it = m_knownItems.begin(); it != m_knownItems.end(); ++it)
    {
        if (it.value().erase(aEntity) && it.key() != apExcludedPlayer)
            it.key()->Send(notify);
    }

    m_world.destroy(aEntity);
}

void DroppedItemService::StopSimulation(entt::entity aEntity, DroppedItemComponent& aDroppedItem) noexcept
{
    const Player* pSimulator = aDroppedItem.pSimulator;
    aDroppedItem.pSimulator = nullptr;

    NotifyDroppedItemMove notify{};
    notify.Move.ServerId = World::ToInteger(aEntity);
    notify.Move.Transform = aDroppedItem.Transform;
    notify.Move.IsAtRest = true;

    SendToPlayersWithItem(notify, aEntity, pSimulator);
}

void DroppedItemService::SendToPlayersWithItem(const ServerMessage& acMessage, entt::entity aEntity, const Player* apExcludedPlayer) const noexcept
{
    for (const auto& [pPlayer, knownItems] : m_knownItems)
    {
        if (pPlayer != apExcludedPlayer && knownItems.contains(aEntity))
            pPlayer->Send(acMessage);
    }
}

DroppedItemData DroppedItemService::ToData(entt::entity aEntity) const noexcept
{
    const auto& droppedItem = m_world.get<DroppedItemComponent>(aEntity);
    const auto& cellComponent = m_world.get<CellIdComponent>(aEntity);

    DroppedItemData data{};
    data.ServerId = World::ToInteger(aEntity);
    data.Item = droppedItem.Item;
    data.CellId = cellComponent.Cell;
    data.WorldSpaceId = cellComponent.WorldSpaceId;
    data.Transform = droppedItem.Transform;
    data.IsAtRest = droppedItem.pSimulator == nullptr;

    return data;
}
