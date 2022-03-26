#include "InventoryService.h"

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Events/UpdateEvent.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestCharacterInventoryChanges.h>
#include <Messages/NotifyCharacterInventoryChanges.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/NotifyDrawWeapon.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_objectInventoryConnection = aDispatcher.sink<PacketEvent<RequestObjectInventoryChanges>>().connect<&InventoryService::OnObjectInventoryChanges>(this);
    m_characterInventoryConnection = aDispatcher.sink<PacketEvent<RequestCharacterInventoryChanges>>().connect<&InventoryService::OnCharacterInventoryChanges>(this);
    m_drawWeaponConnection = aDispatcher.sink<PacketEvent<DrawWeaponRequest>>().connect<&InventoryService::OnWeaponDrawnRequest>(this);
}

void InventoryService::OnUpdate(const UpdateEvent&) noexcept
{
    ProcessObjectInventoryChanges();
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

void InventoryService::OnCharacterInventoryChanges(const PacketEvent<RequestCharacterInventoryChanges>& acMessage) noexcept
{
    auto view = m_world.view<CharacterComponent, InventoryComponent, OwnerComponent>();

    auto& message = acMessage.Packet;

    // TODO: update server inventory

    NotifyCharacterInventoryChanges notify;
    notify.ActorId = message.ActorId;
    notify.Item = message.Item;

    const entt::entity cOrigin = static_cast<entt::entity>(message.ActorId);
    GameServer::Get()->SendToPlayersInRange(notify, cOrigin, acMessage.GetSender());
}

void InventoryService::ProcessObjectInventoryChanges() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 1000ms / 4;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    const auto objectView = m_world.view<FormIdComponent, ObjectComponent, InventoryComponent, CellIdComponent>();

    TiltedPhoques::Map<Player*, NotifyObjectInventoryChanges> messages;

    for (auto entity : objectView)
    {
        auto& formIdComponent = objectView.get<FormIdComponent>(entity);
        auto& inventoryComponent = objectView.get<InventoryComponent>(entity);
        auto& cellIdComponent = objectView.get<CellIdComponent>(entity);
        auto& objectComponent = objectView.get<ObjectComponent>(entity);

        if (inventoryComponent.DirtyInventory == false)
            continue;

        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (pPlayer == objectComponent.pLastSender)
                continue;

            if (!cellIdComponent.IsInRange(pPlayer->GetCellComponent()))
                continue;

            auto& message = messages[pPlayer];
            auto& change = message.Changes[formIdComponent.Id];

            change = inventoryComponent.Content;
        }

        inventoryComponent.DirtyInventory = false;
    }

    for (auto [pPlayer, message] : messages)
    {
        if (!message.Changes.empty())
            pPlayer->Send(message);
    }
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

