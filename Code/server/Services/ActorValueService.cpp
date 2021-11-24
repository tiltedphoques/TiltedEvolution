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
    aDispatcher.sink<PacketEvent<RequestDeathStateChange>>().connect<&ActorValueService::OnDeathStateChange>(this);
}

ActorValueService::~ActorValueService() noexcept
{
}

void ActorValueService::OnActorValueChanges(const PacketEvent<RequestActorValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto itor = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(actorValuesView) &&
        actorValuesView.get<OwnerComponent>(*itor).GetOwner() == acMessage.pPlayer)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorValuesList[id] = value;
            auto val = actorValuesComponent.CurrentActorValues.ActorValuesList[id];
            spdlog::debug("Updating value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorValueChanges notifyChanges;
    notifyChanges.Id = acMessage.Packet.Id;
    notifyChanges.Values = acMessage.Packet.Values;

    for (auto pPlayer : m_world.GetPlayerManager())
    { 
        if (acMessage.pPlayer != pPlayer)
        {
            pPlayer->Send(notifyChanges);
        }
    }
}

void ActorValueService::OnActorMaxValueChanges(const PacketEvent<RequestActorMaxValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto itor = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(actorValuesView) &&
        actorValuesView.get<OwnerComponent>(*itor).GetOwner() == acMessage.pPlayer)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id] = value;
            auto val = actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id];
            spdlog::debug("Updating max value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorMaxValueChanges notifyChanges;
    notifyChanges.Id = message.Id;
    notifyChanges.Values = message.Values;

    for (auto pPlayer : m_world.GetPlayerManager())
    {
        if (acMessage.pPlayer != pPlayer)
        {
            pPlayer->Send(notifyChanges);
        }
    }
}

void ActorValueService::OnHealthChangeBroadcast(const PacketEvent<RequestHealthChangeBroadcast>& acMessage) const noexcept
{
    NotifyHealthChangeBroadcast notifyDamageEvent;
    notifyDamageEvent.Id = acMessage.Packet.Id;
    notifyDamageEvent.DeltaHealth = acMessage.Packet.DeltaHealth;

    for (auto pPlayer : m_world.GetPlayerManager())
    {
        if (acMessage.pPlayer != pPlayer)
        {
            pPlayer->Send(notifyDamageEvent);
        }
    }
}

void ActorValueService::OnDeathStateChange(const PacketEvent<RequestDeathStateChange>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto characterView = m_world.view<CharacterComponent, OwnerComponent>();

    const auto itor = characterView.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(characterView) && 
        characterView.get<OwnerComponent>(*itor).GetOwner() == acMessage.pPlayer)
    {
        auto& characterComponent = characterView.get<CharacterComponent>(*itor);
        characterComponent.IsDead = message.IsDead;
        spdlog::debug("Updating death state {:x}:{}", message.Id, message.IsDead);
    }

    NotifyDeathStateChange notifyChange;
    notifyChange.Id = message.Id;
    notifyChange.IsDead = message.IsDead;

    for (auto pPlayer : m_world.GetPlayerManager())
    {
        if (acMessage.pPlayer != pPlayer)
        {
            pPlayer->Send(notifyChange);
        }
    }
}

