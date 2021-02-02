#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;

struct ConnectedEvent;
struct DisconnectedEvent;
struct ReferenceSpawnedEvent;
struct ReferenceRemovedEvent;
struct UpdateEvent;
struct HealthChangeEvent;

struct TransportService;
struct NotifyActorValueChanges;
struct NotifyActorMaxValueChanges;
struct NotifyHealthChangeBroadcast;

struct Actor;

struct ActorService
{
  public:
    ActorService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport) noexcept;
    ~ActorService() noexcept;

    TP_NOCOPYMOVE(ActorService);

private:

    World& m_world;
    TransportService& m_transport;

    Map<uint32_t, Map<uint32_t, float>> m_actorValues;
    Map<uint32_t, Map<uint32_t, float>> m_actorMaxValues;
    double m_timeSinceDiff = 1;
    
    void OnLocalComponentAdded(entt::registry& aRegistry, entt::entity aEntity) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnReferenceSpawned(const ReferenceSpawnedEvent&) noexcept;
    void OnReferenceRemoved(const ReferenceRemovedEvent&) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnActorValueChanges(const NotifyActorValueChanges& acMessage) noexcept;
    void OnActorMaxValueChanges(const NotifyActorMaxValueChanges& acMessage) noexcept;
    void OnHealthChange(const HealthChangeEvent&) noexcept;
    void OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acMessage) noexcept;

    void AddToActorMap(uint32_t aId, Actor* aActor) noexcept;
    void ForceActorValue(Actor* aActor, uint32_t aMode, uint32_t aId, float aValue) noexcept;
    void SetActorValue(Actor* aActor, uint32_t aId, float aValue) noexcept;
    float GetActorValue(Actor* aActor, uint32_t aId) noexcept;
    float GetActorMaxValue(Actor* aActor, uint32_t aId) noexcept;
};
