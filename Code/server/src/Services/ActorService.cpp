#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValueChanges.h>
#include <Services/ActorService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValueChanges.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept
    : m_world(aWorld)
{
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorService::OnActorValueChanges>(this);
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
