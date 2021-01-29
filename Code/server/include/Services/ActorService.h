#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct TransportService;
struct RequestActorValueChanges;
struct RequestDamageEvent;

struct ActorService
{
    ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept;
    ~ActorService() noexcept;

    TP_NOCOPYMOVE(ActorService);

  private:
    World& m_world;

    void OnActorValueChanges(const PacketEvent<RequestActorValueChanges>& acMessage) const noexcept;
    void OnDamageEvent(const PacketEvent<RequestDamageEvent>& acMessage) const noexcept;

    entt::scoped_connection m_updateHealthConnection;
    entt::scoped_connection m_updateDamageConnection;
};
