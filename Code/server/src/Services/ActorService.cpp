#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Services/ActorService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept
    : m_world(aWorld)
{
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorService::OnActorValueChanges>(this);
    m_updateMaxValueConnection = aDispatcher.sink<PacketEvent<RequestActorMaxValueChanges>>().connect<&ActorService::OnActorMaxValueChanges>(this);
    m_updateDeltaHealthConnection = aDispatcher.sink<PacketEvent<RequestHealthChangeBroadcast>>().connect<&ActorService::OnHealthChangeBroadcast>(this);
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

void ActorService::OnActorMaxValueChanges(const PacketEvent<RequestActorMaxValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto itor = actorValuesView.find(static_cast<entt::entity>(message.m_Id));

    if (itor != std::end(actorValuesView) ||
        actorValuesView.get<OwnerComponent>(*itor).ConnectionId == acMessage.ConnectionId)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
        for (auto& [id, value] : message.m_values)
        {
#if TP_SKYRIM64
            if (value.first != 24)
                continue;
#elif TP_FALLOUT4
            if (value.first != 27)
                continue;
#endif
            actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id] = value;
        }
    }

    NotifyActorMaxValueChanges notifyChanges;
    notifyChanges.m_Id = message.m_Id;
    notifyChanges.m_values = message.m_values;

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

    auto actorValuesView = m_world.view<FormIdComponent, ActorValuesComponent>();

    uint32_t refId = acMessage.Packet.m_Id;

    const auto itor = std::find_if(std::begin(actorValuesView), std::end(actorValuesView), [actorValuesView, refId](auto entity)
        {
            const auto& formIdComponent = actorValuesView.get<FormIdComponent>(entity);

            return formIdComponent.Id.ModId == refId;
        });

    if (itor != std::end(actorValuesView))
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
#if TP_SKYRIM
        float currentHealth = actorValuesComponent.CurrentActorValues.ActorValuesList[24];
        actorValuesComponent.CurrentActorValues.ActorValuesList[24] = currentHealth - acMessage.Packet.m_DeltaHealth;
#elif TP_FALLOUT4
        float currentHealth = actorValuesComponent.CurrentActorValues.ActorValuesList[27];
        actorValuesComponent.CurrentActorValues.ActorValuesList[27] = currentHealth - acMessage.Packet.m_DeltaHealth;
#endif
    }

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

