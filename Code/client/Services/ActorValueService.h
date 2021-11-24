#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;

struct ConnectedEvent;
struct DisconnectedEvent;
struct ReferenceRemovedEvent;
struct UpdateEvent;
struct HealthChangeEvent;

struct TransportService;
struct NotifyActorValueChanges;
struct NotifyActorMaxValueChanges;
struct NotifyHealthChangeBroadcast;
struct NotifyDeathStateChange;

struct Actor;

struct ActorValueService
{
  public:
    ActorValueService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~ActorValueService() noexcept = default;

    TP_NOCOPYMOVE(ActorValueService);

private:
    enum ValueType : uint8_t
    {
        kValue,
        kMaxValue
    };

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    Map<uint32_t, float> m_smallHealthChanges;
    double m_timeSinceDiff = 1;
    
    void OnLocalComponentAdded(entt::registry& aRegistry, entt::entity aEntity) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnReferenceRemoved(const ReferenceRemovedEvent&) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnActorValueChanges(const NotifyActorValueChanges& acMessage) const noexcept;
    void OnActorMaxValueChanges(const NotifyActorMaxValueChanges& acMessage) const noexcept;
    void OnHealthChange(const HealthChangeEvent&) noexcept;
    void OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acMessage) const noexcept;
    void OnDeathStateChange(const NotifyDeathStateChange& acEvent) const noexcept;

    void RunActorValuesUpdates() noexcept;
    void RunSmallHealthUpdates() noexcept;
    void RunDeathStateUpdates() noexcept;
    void CreateActorValuesComponent(entt::entity aEntity, Actor* apActor) noexcept;
    void BroadcastActorValues() noexcept;
};
