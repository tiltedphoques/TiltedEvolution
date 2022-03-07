#include <stdafx.h>
#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Messages/RequestDeathStateChange.h>
#include <Services/ActorValueService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Messages/NotifyDeathStateChange.h>

ActorValueService::ActorValueService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorValueService::OnActorValueChanges>(this);
    m_updateMaxValueConnection = aDispatcher.sink<PacketEvent<RequestActorMaxValueChanges>>().connect<&ActorValueService::OnActorMaxValueChanges>(this);
    m_updateDeltaHealthConnection = aDispatcher.sink<PacketEvent<RequestHealthChangeBroadcast>>().connect<&ActorValueService::OnHealthChangeBroadcast>(this);
    m_deathStateConnection = aDispatcher.sink<PacketEvent<RequestDeathStateChange>>().connect<&ActorValueService::OnDeathStateChange>(this);
}

void ActorValueService::OnActorValueChanges(const PacketEvent<RequestActorValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto it = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (it != std::end(actorValuesView) &&
        actorValuesView.get<OwnerComponent>(*it).GetOwner() == acMessage.pPlayer)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*it);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorValuesList[id] = value;
            float val = actorValuesComponent.CurrentActorValues.ActorValuesList[id];
            spdlog::debug("Updating value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorValueChanges notify;
    notify.Id = acMessage.Packet.Id;
    notify.Values = acMessage.Packet.Values;

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void ActorValueService::OnActorMaxValueChanges(const PacketEvent<RequestActorMaxValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto it = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (it != std::end(actorValuesView) &&
        actorValuesView.get<OwnerComponent>(*it).GetOwner() == acMessage.pPlayer)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*it);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id] = value;
            float val = actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id];
            spdlog::debug("Updating max value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorMaxValueChanges notify;
    notify.Id = message.Id;
    notify.Values = message.Values;

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void ActorValueService::OnHealthChangeBroadcast(const PacketEvent<RequestHealthChangeBroadcast>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyHealthChangeBroadcast notify;
    notify.Id = message.Id;
    notify.DeltaHealth = message.DeltaHealth;

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void ActorValueService::OnDeathStateChange(const PacketEvent<RequestDeathStateChange>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto characterView = m_world.view<CharacterComponent, OwnerComponent>();

    const auto it = characterView.find(static_cast<entt::entity>(message.Id));

    if (it != std::end(characterView) && 
        characterView.get<OwnerComponent>(*it).GetOwner() == acMessage.pPlayer)
    {
        auto& characterComponent = characterView.get<CharacterComponent>(*it);
        characterComponent.IsDead = message.IsDead;
        spdlog::debug("Updating death state {:x}:{}", message.Id, message.IsDead);
    }

    NotifyDeathStateChange notify;
    notify.Id = message.Id;
    notify.IsDead = message.IsDead;

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

