#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestDamageEvent.h>
#include <Services/ActorService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyDamageEvent.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept
    : m_world(aWorld)
{
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorService::OnActorValueChanges>(this);
    m_updateDamageConnection = aDispatcher.sink<PacketEvent<RequestDamageEvent>>().connect<&ActorService::OnDamageEvent>(this);
}

ActorService::~ActorService() noexcept
{
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

void ActorService::OnDamageEvent(const PacketEvent<RequestDamageEvent>& acMessage) const noexcept
{
    NotifyDamageEvent notifyDamageEvent;
    notifyDamageEvent.m_Id = acMessage.Packet.m_Id;
    notifyDamageEvent.m_Damage = acMessage.Packet.m_Damage;

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

