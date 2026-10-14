#include <Services/DroppedItemService.h>

#include <GameServer.h>
#include <World.h>
#include <Components.h>

#include <Messages/DropItemRequest.h>
#include <Messages/DropItemResponse.h>
#include <Messages/PickupDroppedItemRequest.h>
#include <Messages/PickupDroppedItemResponse.h>
#include <Messages/NotifyDroppedItems.h>
#include <Messages/NotifyDroppedItemRemoved.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/ShiftGridCellRequest.h>

#include <Setting.h>
namespace
{
Console::Setting bEnableItemDrops{"Gameplay:bEnableItemDrops", "Syncs items dropped by players, so that everyone sees them and only one player can pick them up", true};
Console::Setting uMaxDroppedItems{"Gameplay:uMaxDroppedItems", "How many dropped items the server keeps track of, the oldest ones are forgotten first", 1000u};
} // namespace

DroppedItemService::DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
{
    m_dropItemConnection = aDispatcher.sink<PacketEvent<DropItemRequest>>().connect<&DroppedItemService::OnDropItemRequest>(this);
    m_pickupDroppedItemConnection = aDispatcher.sink<PacketEvent<PickupDroppedItemRequest>>().connect<&DroppedItemService::OnPickupDroppedItemRequest>(this);
    m_interiorCellEnterConnection = aDispatcher.sink<PacketEvent<EnterInteriorCellRequest>>().connect<&DroppedItemService::OnInteriorCellEnter>(this);
    m_exteriorCellEnterConnection = aDispatcher.sink<PacketEvent<EnterExteriorCellRequest>>().connect<&DroppedItemService::OnExteriorCellEnter>(this);
    m_gridCellShiftConnection = aDispatcher.sink<PacketEvent<ShiftGridCellRequest>>().connect<&DroppedItemService::OnGridCellShift>(this);
}

void DroppedItemService::OnDropItemRequest(const PacketEvent<DropItemRequest>& acMessage) noexcept
{
    const auto& message = acMessage.Packet;

    DropItemResponse response{};
    response.RefId = message.RefId;

    // Without a server id the client simply keeps its local item, like in single player.
    if (!bEnableItemDrops || message.Data.Item.Count <= 0 || !message.Data.CellId)
    {
        acMessage.pPlayer->Send(response);
        return;
    }

    const auto cEntity = m_world.create();

    const GridCellCoords coords = GridCellCoords::CalculateGridCellCoords(message.Data.PositionX, message.Data.PositionY);
    m_world.emplace<CellIdComponent>(cEntity, message.Data.CellId, message.Data.WorldSpaceId, coords);

    auto& droppedItemComponent = m_world.emplace<DroppedItemComponent>(cEntity);
    droppedItemComponent.Data = message.Data;
    droppedItemComponent.Data.ServerId = 0;
    droppedItemComponent.DropOrder = m_nextDropOrder++;

    response.ServerId = World::ToInteger(cEntity);
    acMessage.pPlayer->Send(response);

    NotifyDroppedItems notify{};
    notify.Items.push_back(droppedItemComponent.Data);
    notify.Items.back().ServerId = response.ServerId;

    GameServer::Get()->SendToPlayersInRange(notify, cEntity, acMessage.pPlayer);

    spdlog::debug(
        "Player {:X} dropped item {:X}:{:X} x{}, dropped item id {:X}", acMessage.pPlayer->GetId(), message.Data.Item.BaseId.ModId, message.Data.Item.BaseId.BaseId, message.Data.Item.Count,
        response.ServerId);

    EnforceDroppedItemLimit();
}

void DroppedItemService::OnPickupDroppedItemRequest(const PacketEvent<PickupDroppedItemRequest>& acMessage) noexcept
{
    PickupDroppedItemResponse response{};
    response.ServerId = acMessage.Packet.ServerId;

    const auto cEntity = static_cast<entt::entity>(acMessage.Packet.ServerId);

    // The first request to arrive gets the item. Anyone after that picked up a local copy of an item that is already gone.
    response.Accepted = m_world.valid(cEntity) && m_world.all_of<DroppedItemComponent>(cEntity);

    if (response.Accepted)
    {
        spdlog::debug("Player {:X} picked up dropped item {:X}", acMessage.pPlayer->GetId(), acMessage.Packet.ServerId);
        RemoveDroppedItem(cEntity, acMessage.pPlayer);
    }
    else
        spdlog::info("Rejected pickup of dropped item {:X} by player {:X}, it is already gone", acMessage.Packet.ServerId, acMessage.pPlayer->GetId());

    acMessage.pPlayer->Send(response);
}

void DroppedItemService::OnInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept
{
    SendDroppedItemsInRange(acMessage.pPlayer, CellIdComponent{acMessage.Packet.CellId, {}, {}});
}

void DroppedItemService::OnExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept
{
    const auto& message = acMessage.Packet;
    SendDroppedItemsInRange(acMessage.pPlayer, CellIdComponent{message.CellId, message.WorldSpaceId, message.CurrentCoords});
}

void DroppedItemService::OnGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept
{
    const auto& message = acMessage.Packet;
    SendDroppedItemsInRange(acMessage.pPlayer, CellIdComponent{message.PlayerCell, message.WorldSpaceId, message.CenterCoords});
}

void DroppedItemService::SendDroppedItemsInRange(Player* apPlayer, const CellIdComponent& acCell) const noexcept
{
    if (!bEnableItemDrops)
        return;

    NotifyDroppedItems notify{};

    auto view = m_world.view<DroppedItemComponent, CellIdComponent>();
    for (auto entity : view)
    {
        // Same check as GameServer::SendToPlayersInRange, so players entering later get what they would have been sent live.
        if (!view.get<CellIdComponent>(entity).IsInRange(acCell, false))
            continue;

        notify.Items.push_back(view.get<DroppedItemComponent>(entity).Data);
        notify.Items.back().ServerId = World::ToInteger(entity);
    }

    if (!notify.Items.empty())
        apPlayer->Send(notify);
}

void DroppedItemService::RemoveDroppedItem(entt::entity aEntity, const Player* apExcludedPlayer) noexcept
{
    NotifyDroppedItemRemoved notify{};
    notify.ServerId = World::ToInteger(aEntity);

    // Sent to everyone, not just players in range: clients that walked away may still track or queue the item.
    GameServer::Get()->SendToPlayers(notify, apExcludedPlayer);

    m_world.destroy(aEntity);
}

void DroppedItemService::EnforceDroppedItemLimit() noexcept
{
    auto view = m_world.view<DroppedItemComponent>();
    const size_t cMaxItems = std::max(uMaxDroppedItems.value_as<uint32_t>(), 1u);

    if (view.size() <= cMaxItems)
        return;

    Vector<entt::entity> entities(view.begin(), view.end());
    std::sort(
        entities.begin(), entities.end(),
        [view](entt::entity aLhs, entt::entity aRhs) { return view.get<DroppedItemComponent>(aLhs).DropOrder < view.get<DroppedItemComponent>(aRhs).DropOrder; });

    const size_t cToRemove = entities.size() - cMaxItems;
    for (size_t i = 0; i < cToRemove; ++i)
        RemoveDroppedItem(entities[i], nullptr);
}
