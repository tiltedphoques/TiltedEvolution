#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;

struct ConnectedEvent;
struct DisconnectedEvent;
struct ReferenceSpawnedEvent;
struct ReferenceRemovedEvent;
struct UpdateEvent;

struct TransportService;
struct NotifyActorValueChanges;

struct Actor;

struct ActorService : BSTEventSink<TESHitEvent>
{
  public:
    ActorService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport) noexcept;
    ~ActorService() noexcept;

    TP_NOCOPYMOVE(ActorService);

private:

    World& m_world;
    TransportService& m_transport;

    Map<uint32_t, Map<uint32_t, float>> m_actorValues;
    double m_timeSinceDiff = 1;
    
    void OnLocalComponentAdded(entt::registry& aRegistry, entt::entity aEntity) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnReferenceSpawned(const ReferenceSpawnedEvent&) noexcept;
    void OnReferenceRemoved(const ReferenceRemovedEvent&) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnActorValueChanges(const NotifyActorValueChanges& acMessage) const noexcept;
    BSTEventResult OnEvent(const TESHitEvent*, const EventDispatcher<TESHitEvent>*) override;

    void AddToActorMap(uint32_t aId, Actor* aActor) noexcept;

    entt::scoped_connection m_updateConnection;
};
