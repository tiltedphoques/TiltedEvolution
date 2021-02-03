#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValuesState.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Services/ActorService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValuesState.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept
    : m_world(aWorld)
{
    aDispatcher.sink<PacketEvent<RequestActorValuesState>>().connect<&ActorService::OnActorValuesState>(this);
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorService::OnActorValueChanges>(this);
    m_updateMaxValueConnection = aDispatcher.sink<PacketEvent<RequestActorMaxValueChanges>>().connect<&ActorService::OnActorMaxValueChanges>(this);
    m_updateDeltaHealthConnection = aDispatcher.sink<PacketEvent<RequestHealthChangeBroadcast>>().connect<&ActorService::OnHealthChangeBroadcast>(this);
}

ActorService::~ActorService() noexcept
{
}

void ActorService::OnActorValuesState(const PacketEvent<RequestActorValuesState>& acMessage) const noexcept
{
    NotifyActorValuesState notifyState;
    notifyState.m_Id = 1;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyState);
        }
    }
}

void ActorService::OnActorValueChanges(const PacketEvent<RequestActorValueChanges>& acMessage) const noexcept
{
    NotifyActorValueChanges notifyChanges;
    notifyChanges.m_Id = acMessage.Packet.m_Id;
    notifyChanges.m_values = acMessage.Packet.m_values;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyChanges);
        }
    }
}

void ActorService::OnActorMaxValueChanges(const PacketEvent<RequestActorMaxValueChanges>& acMessage) const noexcept
{
    NotifyActorMaxValueChanges notifyChanges;
    notifyChanges.m_Id = acMessage.Packet.m_Id;
    notifyChanges.m_values = acMessage.Packet.m_values;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyChanges);
        }
    }
}

void ActorService::OnHealthChangeBroadcast(const PacketEvent<RequestHealthChangeBroadcast>& acMessage) const noexcept
{
    NotifyHealthChangeBroadcast notifyDamageEvent;
    notifyDamageEvent.m_Id = acMessage.Packet.m_Id;
    notifyDamageEvent.m_DeltaHealth = acMessage.Packet.m_DeltaHealth;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyDamageEvent);
        }
    }
}

