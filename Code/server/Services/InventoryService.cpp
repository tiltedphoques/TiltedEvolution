#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/NotifyDrawWeapon.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_objectInventoryConnection = aDispatcher.sink<PacketEvent<RequestObjectInventoryChanges>>().connect<&InventoryService::OnObjectInventoryChanges>(this);
    m_characterInventoryConnection = aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&InventoryService::OnInventoryChanges>(this);
    m_drawWeaponConnection = aDispatcher.sink<PacketEvent<DrawWeaponRequest>>().connect<&InventoryService::OnWeaponDrawnRequest>(this);
}

void InventoryService::OnObjectInventoryChanges(const PacketEvent<RequestObjectInventoryChanges>& acMessage) noexcept
{
    auto& message = acMessage.Packet;

    for (auto& [id, objectData] : message.Changes)
    {
        auto view = m_world.view<FormIdComponent, ObjectComponent>();

        auto formIdIt = std::find_if(std::begin(view), std::end(view), [view, id = id](auto entity) {
            const auto& formIdComponent = view.get<FormIdComponent>(entity);
            return formIdComponent.Id == id;
        });

        if (formIdIt == std::end(view))
        {
            const auto entity = m_world.create();
            m_world.emplace<FormIdComponent>(entity, id.BaseId, id.ModId);
            m_world.emplace<ObjectComponent>(entity, acMessage.pPlayer);
            m_world.emplace<CellIdComponent>(entity, objectData.CellId, objectData.WorldSpaceId, objectData.CurrentCoords);

            auto& inventoryComponent = m_world.emplace<InventoryComponent>(entity);
            inventoryComponent.Content = objectData.CurrentInventory;
            inventoryComponent.DirtyInventory = true;
        }
        else
        {
            auto& objectComponent = m_world.get<ObjectComponent>(*formIdIt);
            objectComponent.pLastSender = acMessage.pPlayer;

            auto& inventoryComponent = m_world.get<InventoryComponent>(*formIdIt);
            inventoryComponent.Content = objectData.CurrentInventory;
            inventoryComponent.DirtyInventory = true;
        }
    }
}

void InventoryService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept
{
    auto view = m_world.view<CharacterComponent, InventoryComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    // TODO: update server inventory

    NotifyInventoryChanges notify;
    notify.ServerId = message.ServerId;
    notify.Item = message.Item;

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

