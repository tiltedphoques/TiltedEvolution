#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/RequestEquipmentChanges.h>
#include <Messages/NotifyEquipmentChanges.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/NotifyDrawWeapon.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_inventoryChangeConnection = aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&InventoryService::OnInventoryChanges>(this);
    m_equipmentChangeConnection = aDispatcher.sink<PacketEvent<RequestEquipmentChanges>>().connect<&InventoryService::OnEquipmentChanges>(this);
    m_drawWeaponConnection = aDispatcher.sink<PacketEvent<DrawWeaponRequest>>().connect<&InventoryService::OnWeaponDrawnRequest>(this);
}

void InventoryService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto view = m_world.view<InventoryComponent>();

    const auto it = view.find(static_cast<entt::entity>(message.ServerId));

    if (it != view.end())
    {
        auto& inventoryComponent = view.get<InventoryComponent>(*it);
        inventoryComponent.Content.AddOrRemoveEntry(message.Item);
    }

    NotifyInventoryChanges notify;
    notify.ServerId = message.ServerId;
    notify.Item = message.Item;
    notify.DropOrPickUp = message.DropOrPickUp;

    const entt::entity cOrigin = static_cast<entt::entity>(message.ServerId);
    GameServer::Get()->SendToPlayersInRange(notify, cOrigin, acMessage.GetSender());
}

void InventoryService::OnEquipmentChanges(const PacketEvent<RequestEquipmentChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto view = m_world.view<InventoryComponent>();

    const auto it = view.find(static_cast<entt::entity>(message.ServerId));

    if (it != view.end())
    {
        auto& inventoryComponent = view.get<InventoryComponent>(*it);
        inventoryComponent.Content.CurrentEquipment.UpdateEquipment(message);
        spdlog::debug("Updating equipment, RightHandSpell: {:X}, RightHandWeapon: {:X}, LeftHandSpell: {:X}, LeftHandWeapon: {:X}, Shout: {:X}, Ammo: {:X}",
                     inventoryComponent.Content.CurrentEquipment.RightHandSpell.BaseId, inventoryComponent.Content.CurrentEquipment.RightHandWeapon.BaseId, 
                     inventoryComponent.Content.CurrentEquipment.LeftHandSpell.BaseId, inventoryComponent.Content.CurrentEquipment.LeftHandWeapon.BaseId,
                     inventoryComponent.Content.CurrentEquipment.Shout.BaseId, inventoryComponent.Content.CurrentEquipment.Ammo.BaseId);
    }

    NotifyEquipmentChanges notify;
    notify.ServerId = message.ServerId;
    notify.ItemId = message.ItemId;
    notify.EquipSlotId = message.EquipSlotId;
    notify.Count = message.Count;
    notify.Unequip = message.Unequip;
    notify.IsSpell = message.IsSpell;
    notify.IsShout = message.IsShout;

    const entt::entity cOrigin = static_cast<entt::entity>(message.ServerId);
    GameServer::Get()->SendToPlayersInRange(notify, cOrigin, acMessage.GetSender());
}

void InventoryService::OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    auto characterView = m_world.view<CharacterComponent, OwnerComponent>();
    const auto it = characterView.find(static_cast<entt::entity>(message.Id));

    if (it != std::end(characterView) 
        && characterView.get<OwnerComponent>(*it).GetOwner() == acMessage.pPlayer)
    {
        auto& characterComponent = characterView.get<CharacterComponent>(*it);
        characterComponent.IsWeaponDrawn = message.IsWeaponDrawn;
        spdlog::debug("Updating weapon drawn state {:x}:{}", message.Id, message.IsWeaponDrawn);
    }

    NotifyDrawWeapon notify;
    notify.Id = message.Id;
    notify.IsWeaponDrawn = message.IsWeaponDrawn;

    const entt::entity cEntity = static_cast<entt::entity>(message.Id);
    GameServer::Get()->SendToPlayersInRange(notify, cEntity, acMessage.GetSender());
}

