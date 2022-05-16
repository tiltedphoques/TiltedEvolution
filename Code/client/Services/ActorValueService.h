#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;

struct ConnectedEvent;
struct DisconnectedEvent;
struct ActorRemovedEvent;
struct UpdateEvent;
struct HealthChangeEvent;

struct TransportService;
struct NotifyActorValueChanges;
struct NotifyActorMaxValueChanges;
struct NotifyHealthChangeBroadcast;
struct NotifyDeathStateChange;

struct Actor;

/**
* @brief Broadcast and apply actor values and death state.
* 
* Responsible for periodically checking whether actor values of local actors have changed since
* the previous frame, and if so, broadcast those values to other players within range.
* This service is also responsible for applying incoming actor value changes.
* Health is synced separately from other actor values, since health changes
* need to be synced immediately.
* 
* This service is also responsible for syncing the death states of individual actors.
* The death state is perdiocally checked and compared to cached death states.
*/
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

    /**
    * @brief Creates actor values components for new local components.
    */
    void OnLocalComponentAdded(entt::registry& aRegistry, entt::entity aEntity) noexcept;
    /**
    * @brief Clears actor values components on disconnect.
    */
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    /**
    * @brief Removes actor values component on reference removed.
    */
    void OnActorRemoved(const ActorRemovedEvent&) noexcept;
    /**
    * @brief Checks whether health, death state and actor values should be broadcasted.
    */
    void OnUpdate(const UpdateEvent&) noexcept;
    /**
    * @brief Receives remote actor values changes and applies them locally.
    */
    void OnActorValueChanges(const NotifyActorValueChanges& acMessage) const noexcept;
    /**
    * @brief Receives remote max actor values changes and applies them locally.
    */
    void OnActorMaxValueChanges(const NotifyActorMaxValueChanges& acMessage) const noexcept;
    /**
    * @brief Processes health value changes.
    * 
    * If the health changes are small, it will be cached and sent out on update.
    */
    void OnHealthChange(const HealthChangeEvent&) noexcept;
    /**
    * @brief Receives health value changes and applies them locally.
    */
    void OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acMessage) const noexcept;
    /**
    * @brief Receives death state changes and applies them locally.
    */
    void OnDeathStateChange(const NotifyDeathStateChange& acEvent) const noexcept;

    /**
    * @brief Checks and broadcasts new actor values.
    * 
    * If the current (max) actor values have changed compared to the previously cached actor values,
    * the new actor values will be broadcasted to the other clients in range.
    */
    void RunActorValuesUpdates() noexcept;
    /**
    * @brief Checks and broadcasts small health changes.
    * 
    * If the small health member variable has collected any health changes, this function
    * broadcasts those changes to the other clients.
    */
    void RunSmallHealthUpdates() noexcept;
    /**
    * @brief Checks and broadcasts death state changes.
    */
    void RunDeathStateUpdates() noexcept;

    /**
    * @brief Checks and broadcasts actor values.
    * @see RunActorValuesUpdates()
    */
    void BroadcastActorValues() noexcept;
    /**
    * @brief Creates actor values component for actor and caches the initial actor values.
    */
    void CreateActorValuesComponent(entt::entity aEntity, Actor* apActor) noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    //! @brief Server ids and collected health changes.
    Map<uint32_t, float> m_smallHealthChanges;
};
