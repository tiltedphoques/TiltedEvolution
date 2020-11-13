#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;
struct UpdateEvent;
struct TransportService;
struct NotifyActorValueChanges;

struct ActorService : BSTEventSink<TESHitEvent>
{
  public:
    ActorService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport) noexcept;
    ~ActorService() noexcept;

    TP_NOCOPYMOVE(ActorService);

private:

    World& m_world;
    TransportService& m_transport;
    
    void UpdateHealth(const UpdateEvent&) noexcept;
    void OnActorValueChanges(const NotifyActorValueChanges& acMessage) const noexcept;
    BSTEventResult OnEvent(const TESHitEvent*, const EventDispatcher<TESHitEvent>*) override;

    entt::scoped_connection m_updateHealthConnection;
};
