#include <TiltedOnlinePCH.h>

#include <Services/ActorValueService.h>
#include <World.h>
#include <Forms/ActorValueInfo.h>
#include <Games/References.h>
#include <Components.h>

#include <Events/UpdateEvent.h>
#include <Events/ReferenceSpawnedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/HealthChangeEvent.h>

#include <Messages/NotifyActorValueChanges.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Messages/NotifyDeathStateChange.h>
#include <Messages/RequestDeathStateChange.h>

#include <misc/ActorValueOwner.h>

ActorValueService::ActorValueService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_world.on_construct<LocalComponent>().connect<&ActorValueService::OnLocalComponentAdded>(this);
    m_dispatcher.sink<DisconnectedEvent>().connect<&ActorValueService::OnDisconnected>(this);
    m_dispatcher.sink<ReferenceSpawnedEvent>().connect<&ActorValueService::OnReferenceSpawned>(this);
    m_dispatcher.sink<ReferenceRemovedEvent>().connect<&ActorValueService::OnReferenceRemoved>(this);
    m_dispatcher.sink<UpdateEvent>().connect<&ActorValueService::OnUpdate>(this);
    m_dispatcher.sink<NotifyActorValueChanges>().connect<&ActorValueService::OnActorValueChanges>(this);
    m_dispatcher.sink<NotifyActorMaxValueChanges>().connect<&ActorValueService::OnActorMaxValueChanges>(this);
    m_dispatcher.sink<HealthChangeEvent>().connect<&ActorValueService::OnHealthChange>(this);
    m_dispatcher.sink<NotifyHealthChangeBroadcast>().connect<&ActorValueService::OnHealthChangeBroadcast>(this);
    m_dispatcher.sink<NotifyDeathStateChange>().connect<&ActorValueService::OnDeathStateChange>(this);
}

void ActorValueService::CreateActorValuesComponent(const entt::entity aEntity, Actor* apActor) noexcept
{
    auto& actorValuesComponent = m_world.emplace<ActorValuesComponent>(aEntity);

    int amountOfValues;
#if TP_SKYRIM64
    amountOfValues = 164;
#elif TP_FALLOUT4
    amountOfValues = 132;
#endif

    for (int i = 0; i < amountOfValues; i++)
    {
#if TP_FALLOUT4
        // These indices in the ActorValueInfo list are null
        if (i == 23 || i == 48 || i == 70)
            continue;
#endif

        float value = apActor->GetActorValue(i);
        actorValuesComponent.CurrentActorValues.ActorValuesList.insert({i, value});
        float maxValue = apActor->GetActorMaxValue(i);
        actorValuesComponent.CurrentActorValues.ActorMaxValuesList.insert({i, maxValue});
    }
}

void ActorValueService::OnLocalComponentAdded(entt::registry& aRegistry, const entt::entity aEntity) noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);
    auto* pForm = TESForm::GetById(formIdComponent.Id);
    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

    if (pActor != NULL)
    {
        auto& localComponent = aRegistry.get<LocalComponent>(aEntity);
        localComponent.IsDead = pActor->IsDead();
        CreateActorValuesComponent(aEntity, pActor);
    }
}

void ActorValueService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    m_world.clear<ActorValuesComponent>();
}

void ActorValueService::OnReferenceSpawned(const ReferenceSpawnedEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto* localComponent = m_world.try_get<LocalComponent>(acEvent.Entity);
    if (localComponent != NULL)
    {
        auto* pForm = TESForm::GetById(acEvent.FormId);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (pActor != NULL)
        {
            const auto entity = entt::to_entity(m_world, localComponent);
            CreateActorValuesComponent(entity, pActor);
        }
    }
}

void ActorValueService::OnReferenceRemoved(const ReferenceRemovedEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();
    const auto formId = acEvent.FormId;

    const auto itor = std::find_if(std::begin(view), std::end(view), [view, formId](auto entity) 
    {
        const auto& formIdComponent = view.get<FormIdComponent>(entity);

        return formIdComponent.Id == formId;
    });

    if (itor != std::end(view))
    {
        m_world.remove_if_exists<ActorValuesComponent>(*itor);
    }
}

void ActorValueService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunSmallHealthUpdates();
    RunDeathStateUpdates();
    m_timeSinceDiff += acEvent.Delta;
    if (m_timeSinceDiff >= 1)
    {
        m_timeSinceDiff = 0;

        BroadcastActorValues();
    }
}

void ActorValueService::BroadcastActorValues() noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent, LocalComponent, ActorValuesComponent>();

    for (auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (!pActor)
            continue;

        auto& localComponent = view.get<LocalComponent>(entity);
        auto& actorValuesComponent = view.get<ActorValuesComponent>(entity);

        RequestActorValueChanges requestValueChanges;
        requestValueChanges.Id = localComponent.Id;
        RequestActorMaxValueChanges requestMaxValueChanges;
        requestMaxValueChanges.Id = localComponent.Id;

        int amountOfValues;
#if TP_SKYRIM64
        amountOfValues = 164;
#elif TP_FALLOUT4
        amountOfValues = 132;
#endif

        for (int i = 0; i < amountOfValues; i++)
        {
#if TP_FALLOUT4
            // These indices in the ActorValueInfo list are null
            if (i == 23 || i == 48 || i == 70)
                continue;
#endif
            float newValue = pActor->GetActorValue(i);
            float oldValue = actorValuesComponent.CurrentActorValues.ActorValuesList[i];
            if (newValue != oldValue)
            {
                requestValueChanges.Values.insert({i, newValue});
                actorValuesComponent.CurrentActorValues.ActorValuesList[i] = newValue;
            }

            float newMaxValue = pActor->GetActorMaxValue(i);
            float oldMaxValue = actorValuesComponent.CurrentActorValues.ActorMaxValuesList[i];
            if (newMaxValue != oldMaxValue)
            {
                requestValueChanges.Values.insert({i, newValue});
                actorValuesComponent.CurrentActorValues.ActorMaxValuesList[i] = newMaxValue;
            }
        }

        if (requestValueChanges.Values.size() > 0)
        {
            m_transport.Send(requestValueChanges);
        }

        if (requestMaxValueChanges.Values.size() > 0)
        {
            m_transport.Send(requestMaxValueChanges);
        }
    }
}

void ActorValueService::OnHealthChange(const HealthChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        if (formIdComponent.Id == acEvent.pHittee->formID)
        {
            if (const auto* pLocalComponent = m_world.try_get<LocalComponent>(entity); pLocalComponent)
            {
                if (acEvent.DeltaHealth > -1.0f && acEvent.DeltaHealth < 1.0f)
                {
                    if (m_smallHealthChanges.find(pLocalComponent->Id) == m_smallHealthChanges.end())
                        m_smallHealthChanges[pLocalComponent->Id] = acEvent.DeltaHealth;
                    else
                        m_smallHealthChanges[pLocalComponent->Id] += acEvent.DeltaHealth;
                    return;
                }

                RequestHealthChangeBroadcast requestHealthChange;
                requestHealthChange.Id = pLocalComponent->Id;
                requestHealthChange.DeltaHealth = acEvent.DeltaHealth;

                m_transport.Send(requestHealthChange);

                spdlog::debug("Sent out delta health through collection: {:x}:{:f}", pLocalComponent->Id, acEvent.DeltaHealth);
            }
            else if (const auto* pRemoteComponent = m_world.try_get<RemoteComponent>(entity); pRemoteComponent)
            {
                if (-1.0f < acEvent.DeltaHealth && acEvent.DeltaHealth < 1.0f)
                {
                    if (m_smallHealthChanges.find(pRemoteComponent->Id) == m_smallHealthChanges.end())
                        m_smallHealthChanges[pRemoteComponent->Id] = acEvent.DeltaHealth;
                    else
                        m_smallHealthChanges[pRemoteComponent->Id] += acEvent.DeltaHealth;
                    return;
                }

                RequestHealthChangeBroadcast requestHealthChange;
                requestHealthChange.Id = pRemoteComponent->Id;
                requestHealthChange.DeltaHealth = acEvent.DeltaHealth;

                m_transport.Send(requestHealthChange);

                spdlog::debug("Sent out delta health through collection: {:x}:{:f}", pRemoteComponent->Id, acEvent.DeltaHealth);
            }
        }
    }
}

void ActorValueService::RunSmallHealthUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_smallHealthChanges.empty())
    {
        for (auto& value : m_smallHealthChanges)
        {
            RequestHealthChangeBroadcast requestHealthChange;
            requestHealthChange.Id = value.first;
            requestHealthChange.DeltaHealth = value.second;

            m_transport.Send(requestHealthChange);

            spdlog::debug("Sent out delta health through timer. {:x}:{:f}", value.first, value.second);
        }

        m_smallHealthChanges.clear();
    }
}

void ActorValueService::RunDeathStateUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    auto view = m_world.view<FormIdComponent, LocalComponent>();

    for (auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        auto& localComponent = view.get<LocalComponent>(entity);

        bool isDead = pActor->IsDead();
        if (isDead != localComponent.IsDead)
        {
            localComponent.IsDead = isDead;

            RequestDeathStateChange requestChange;
            requestChange.Id = localComponent.Id;
            requestChange.IsDead = isDead;

            m_transport.Send(requestChange);
        }
    }
}

void ActorValueService::OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acEvent) const noexcept
{
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        uint32_t componentId;
        const auto cpLocalComponent = m_world.try_get<LocalComponent>(entity);
        const auto cpRemoteComponent = m_world.try_get<RemoteComponent>(entity);

        if (cpLocalComponent)
            componentId = cpLocalComponent->Id;
        else if (cpRemoteComponent)
            componentId = cpRemoteComponent->Id;
        else
            continue;

        if (componentId == acEvent.Id)
        {
            auto& formIdComponent = view.get<FormIdComponent>(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != nullptr)
            {
                const auto newHealth = pActor->GetActorValue(ActorValueInfo::kHealth) + acEvent.DeltaHealth;
                pActor->ForceActorValue(2, ActorValueInfo::kHealth, newHealth);
            }
        }
    }
}

void ActorValueService::OnActorValueChanges(const NotifyActorValueChanges& acEvent) const noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acEvent.Id, view](entt::entity entity) 
    {
        return view.get<RemoteComponent>(entity).Id == id;
    });

    if (itor == std::end(view))
        return;

    auto& formIdComponent = view.get<FormIdComponent>(*itor);
    auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);

    if (!pActor)
        return;

    for (const auto& [key, value] : acEvent.Values)
    {
        std::cout << "Form ID: " << std::hex << formIdComponent.Id << " Remote ID: " << std::hex << acEvent.Id << std::endl;
        std::cout << "Key: " << std::dec << key << " Value: " << value << std::endl;

        if (key == ActorValueInfo::kHealth)
            continue;

#if TP_SKYRIM64
        // Syncing dragon souls triggers "Dragon soul collected" event
        if (key == ActorValueInfo::kDragonSouls)
            continue;

        if (key == ActorValueInfo::kStamina || key == ActorValueInfo::kMagicka)
        {
            pActor->ForceActorValue(2, key, value);
        }
#endif
        else
        {
            pActor->SetActorValue(key, value);
        }
    }
}

void ActorValueService::OnActorMaxValueChanges(const NotifyActorMaxValueChanges& acEvent) const noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acEvent.Id, view](entt::entity entity)
    {
        return view.get<RemoteComponent>(entity).Id == id;
    });

    if (itor == std::end(view))
        return;

    auto& formIdComponent = view.get<FormIdComponent>(*itor);
    auto* pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);

    if (!pActor)
        return;

    for (const auto& [key, value] : acEvent.Values)
    {
#if TP_SKYRIM64
        if (key == ActorValueInfo::kDragonSouls)
            continue;
#endif

        std::cout << "Max values update." << std::endl;
        std::cout << "Form ID: " << std::hex << formIdComponent.Id << " Remote ID: " << std::hex << acEvent.Id << std::endl;
        std::cout << "Key: " << std::dec << key << " Value: " << value << std::endl;

        pActor->ForceActorValue(0, key, value);
    }
}

void ActorValueService::OnDeathStateChange(const NotifyDeathStateChange& acMessage) const noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acMessage.Id, view](entt::entity entity) {
        return view.get<RemoteComponent>(entity).Id == id;
    });

    if (itor == std::end(view))
        return;

    auto& formIdComponent = view.get<FormIdComponent>(*itor);
    auto* pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);

    if (!pActor)
        return;

    if (pActor->IsDead() != acMessage.IsDead)
        acMessage.IsDead ? pActor->Kill() : pActor->Respawn();
}
