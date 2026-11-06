#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/RequestEquipmentChanges.h>
#include <Messages/NotifyEquipmentChanges.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/RequestDroppedItemPickup.h>
#include <Messages/NotifyDroppedItemPickup.h>
#include <Messages/NotifyDroppedItem.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/ShiftGridCellRequest.h>

#include <Setting.h>
namespace
{
#if defined(TP_FALLOUT4)
Console::Setting bEnableItemDrops{"Gameplay:bEnableItemDrops", "Syncs dropped items by players", true};
#else
Console::Setting bEnableItemDrops{"Gameplay:bEnableItemDrops", "(Experimental) Syncs dropped items by players", false};
#endif

// Oldest drops are forgotten past this, the game cleans them up on cell reset anyway
constexpr size_t cMaxDroppedItems = 512;
}

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
{
    m_inventoryChangeConnection = aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&InventoryService::OnInventoryChanges>(this);
    m_equipmentChangeConnection = aDispatcher.sink<PacketEvent<RequestEquipmentChanges>>().connect<&InventoryService::OnEquipmentChanges>(this);
    m_drawWeaponConnection = aDispatcher.sink<PacketEvent<DrawWeaponRequest>>().connect<&InventoryService::OnWeaponDrawnRequest>(this);
    m_droppedItemPickupConnection = aDispatcher.sink<PacketEvent<RequestDroppedItemPickup>>().connect<&InventoryService::OnDroppedItemPickup>(this);
    m_exteriorCellEnterConnection = aDispatcher.sink<PacketEvent<EnterExteriorCellRequest>>().connect<&InventoryService::OnExteriorCellEnter>(this);
    m_interiorCellEnterConnection = aDispatcher.sink<PacketEvent<EnterInteriorCellRequest>>().connect<&InventoryService::OnInteriorCellEnter>(this);
    m_gridCellShiftConnection = aDispatcher.sink<PacketEvent<ShiftGridCellRequest>>().connect<&InventoryService::OnGridCellShift>(this);
}

void InventoryService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto view = m_world.view<InventoryComponent>();

    const auto it = view.find(static_cast<entt::entity>(message.ServerId));

    if (it == view.end())
        return;

    bool isRemoteNpcInteraction = false;

    const auto* pOwnerComponent = m_world.try_get<OwnerComponent>(*it);
    if (pOwnerComponent)
    {
        const auto* pOwner = pOwnerComponent->GetOwner();
        if (pOwnerComponent->OwnershipEpoch != message.OwnershipEpoch)
        {
            const uint32_t ownerId = pOwner ? pOwner->GetId() : 0;
            spdlog::debug(
                "Rejected inventory change from player {:X} for actor {:X} at stale epoch {}; current owner is {:X} at epoch {}",
                acMessage.pPlayer->GetId(), message.ServerId, message.OwnershipEpoch, ownerId, pOwnerComponent->OwnershipEpoch);
            return;
        }

        if (pOwner != acMessage.pPlayer)
        {
            const auto* pCharacterComponent = m_world.try_get<CharacterComponent>(*it);
            const auto* pCellComponent = m_world.try_get<CellIdComponent>(*it);
            // A non-owner may still change an NPC's inventory through normal gameplay interactions
            // such as pickpocketing or looting. The epoch and range checks keep the interaction tied
            // to the currently visible incarnation of that NPC.
            isRemoteNpcInteraction = pOwner && pCharacterComponent && pCellComponent && !pCharacterComponent->IsPlayer()
                && acMessage.pPlayer->GetCellComponent().IsInRange(*pCellComponent, pCharacterComponent->IsDragon());

            if (!isRemoteNpcInteraction)
            {
                const uint32_t ownerId = pOwner ? pOwner->GetId() : 0;
                spdlog::debug(
                    "Rejected inventory change from player {:X} for actor {:X} because it is owned by player {:X}", acMessage.pPlayer->GetId(), message.ServerId, ownerId);
                return;
            }
        }
    }
    else if (message.OwnershipEpoch != 0)
    {
        spdlog::warn(
            "Rejected inventory change from player {:X} because object {:X} unexpectedly carried ownership epoch {}",
            acMessage.pPlayer->GetId(), message.ServerId, message.OwnershipEpoch);
        return;
    }

    auto& inventoryComponent = view.get<InventoryComponent>(*it);
    inventoryComponent.Content.AddOrRemoveEntry(message.Item);

#if defined(TP_FALLOUT4)
    if (bEnableItemDrops && message.Drop && message.DropId && !isRemoteNpcInteraction)
        AddDroppedItem(*it, message, acMessage.pPlayer);
#endif

    if (!message.UpdateClients && !isRemoteNpcInteraction)
        return;

    NotifyInventoryChanges notify;
    notify.ServerId = message.ServerId;
    notify.OwnershipEpoch = message.OwnershipEpoch;
    notify.Item = message.Item;

#if !defined(TP_FALLOUT4)
    // Fallout 4 sends dropped items on their own, see AddDroppedItem
    notify.Drop = bEnableItemDrops && !isRemoteNpcInteraction ? message.Drop : false;
#endif

    const entt::entity cOrigin = static_cast<entt::entity>(message.ServerId);
    if (!GameServer::Get()->SendToPlayersInRange(notify, cOrigin, acMessage.GetSender()))
        spdlog::error("{}: SendToPlayersInRange failed", __FUNCTION__);
}

void InventoryService::OnEquipmentChanges(const PacketEvent<RequestEquipmentChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto view = m_world.view<InventoryComponent>();

    const auto it = view.find(static_cast<entt::entity>(message.ServerId));

    if (it == view.end())
        return;

    const auto* pOwnerComponent = m_world.try_get<OwnerComponent>(*it);
    if (pOwnerComponent)
    {
        if (pOwnerComponent->GetOwner() != acMessage.pPlayer || pOwnerComponent->OwnershipEpoch != message.OwnershipEpoch)
        {
            const uint32_t ownerId = pOwnerComponent->GetOwner() ? pOwnerComponent->GetOwner()->GetId() : 0;
            spdlog::debug(
                "Rejected equipment change from player {:X} for actor {:X}; current owner is {:X} and requested epoch {} does not match {}",
                acMessage.pPlayer->GetId(), message.ServerId, ownerId, message.OwnershipEpoch, pOwnerComponent->OwnershipEpoch);
            return;
        }
    }
    else if (message.OwnershipEpoch != 0)
    {
        spdlog::warn(
            "Rejected equipment change from player {:X} because object {:X} unexpectedly carried ownership epoch {}",
            acMessage.pPlayer->GetId(), message.ServerId, message.OwnershipEpoch);
        return;
    }

    auto& inventoryComponent = view.get<InventoryComponent>(*it);
    inventoryComponent.Content.UpdateEquipment(message.CurrentInventory);
    if (!message.Unequip && !message.Mods.empty())
    {
        for (auto& entry : inventoryComponent.Content.Entries)
        {
            if (entry.BaseId == message.ItemId)
                entry.Mods = message.Mods;
        }
    }

    NotifyEquipmentChanges notify;
    notify.ServerId = message.ServerId;
    notify.OwnershipEpoch = message.OwnershipEpoch;
    notify.ItemId = message.ItemId;
    notify.EquipSlotId = message.EquipSlotId;
    notify.Count = message.Count;
    notify.Unequip = message.Unequip;
    notify.IsSpell = message.IsSpell;
    notify.IsShout = message.IsShout;
    notify.Mods = message.Mods;

    const entt::entity cOrigin = static_cast<entt::entity>(message.ServerId);
    if (!GameServer::Get()->SendToPlayersInRange(notify, cOrigin, acMessage.GetSender()))
        spdlog::error("{}: SendToPlayersInRange failed", __FUNCTION__);
}

void InventoryService::OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto characterView = m_world.view<CharacterComponent, OwnerComponent>();
    const auto it = characterView.find(static_cast<entt::entity>(message.Id));

    if (it != std::end(characterView) && characterView.get<OwnerComponent>(*it).GetOwner() == acMessage.pPlayer)
    {
        auto& characterComponent = characterView.get<CharacterComponent>(*it);
        characterComponent.SetWeaponDrawn(message.IsWeaponDrawn);
        spdlog::debug("Updating weapon drawn state {:x}:{}", message.Id, message.IsWeaponDrawn);
    }
}

void InventoryService::OnDroppedItemPickup(const PacketEvent<RequestDroppedItemPickup>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    NotifyDroppedItemPickup notify;
    notify.DropperId = message.DropperId;
    notify.DropId = message.DropId;

    const auto it = std::find_if(m_droppedItems.begin(), m_droppedItems.end(), [&message](const DroppedItem& acItem)
                                 { return acItem.DropperId == message.DropperId && acItem.DropId == message.DropId; });

    // Someone else picked it up first
    if (it == m_droppedItems.end())
    {
        notify.Result = NotifyDroppedItemPickup::kDenied;
        acMessage.pPlayer->Send(notify);
        spdlog::info("Denied pickup of dropped item {:X}:{:X}, already taken", message.DropperId, message.DropId);
        return;
    }

    m_droppedItems.erase(it);
    spdlog::info("Dropped item {:X}:{:X} picked up", message.DropperId, message.DropId);

    // Clients that never saw the drop ignore it
    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);

    notify.Result = NotifyDroppedItemPickup::kConfirmed;
    acMessage.pPlayer->Send(notify);
}

void InventoryService::AddDroppedItem(entt::entity aDropper, const RequestInventoryChanges& acMessage, const Player* apSender) noexcept
{
    const auto* pCell = m_world.try_get<CellIdComponent>(aDropper);
    if (!pCell || acMessage.Item.Count >= 0)
    {
        spdlog::warn("Ignored drop {:X}:{:X}, cell component {}", acMessage.ServerId, acMessage.DropId, pCell != nullptr);
        return;
    }

    if (m_droppedItems.size() >= cMaxDroppedItems)
        m_droppedItems.erase(m_droppedItems.begin());

    auto& item = m_droppedItems.emplace_back();
    item.DropperId = acMessage.ServerId;
    item.DropId = acMessage.DropId;
    item.Item = acMessage.Item;
    item.Item.Count = -acMessage.Item.Count;
    item.Position = acMessage.DropPosition;
    item.Cell = *pCell;

    spdlog::info("Item {:X} x{} dropped as {:X}:{:X}, interior {}", item.Item.BaseId.BaseId, item.Item.Count, item.DropperId, item.DropId, item.Cell.IsInInteriorCell());

    NotifyDroppedItem notify;
    Serialize(item, notify);

    if (!GameServer::Get()->SendToPlayersInRange(notify, aDropper, apSender))
        spdlog::error("{}: SendToPlayersInRange failed", __FUNCTION__);
}

void InventoryService::SendDroppedItems(Player* apPlayer, const CellIdComponent& acCell) const noexcept
{
    for (const auto& item : m_droppedItems)
    {
        if (!acCell.IsInRange(item.Cell, false))
            continue;

        NotifyDroppedItem notify;
        Serialize(item, notify);
        apPlayer->Send(notify);
    }
}

void InventoryService::Serialize(const DroppedItem& acItem, NotifyDroppedItem& aNotify) noexcept
{
    aNotify.DropperId = acItem.DropperId;
    aNotify.DropId = acItem.DropId;
    aNotify.Item = acItem.Item;
    aNotify.Position = acItem.Position;
    if (acItem.Cell.IsInInteriorCell())
        aNotify.CellId = acItem.Cell.Cell;
    else
        aNotify.WorldSpaceId = acItem.Cell.WorldSpaceId;
}

void InventoryService::OnExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept
{
    const auto& message = acMessage.Packet;
    SendDroppedItems(acMessage.pPlayer, CellIdComponent{message.CellId, message.WorldSpaceId, message.CurrentCoords});
}

void InventoryService::OnInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept
{
    SendDroppedItems(acMessage.pPlayer, CellIdComponent{acMessage.Packet.CellId, {}, {}});
}

void InventoryService::OnGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept
{
    const auto& message = acMessage.Packet;
    SendDroppedItems(acMessage.pPlayer, CellIdComponent{message.PlayerCell, message.WorldSpaceId, message.CenterCoords});
}
